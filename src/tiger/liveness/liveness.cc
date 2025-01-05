#include "tiger/liveness/liveness.h"
#include <unordered_set>
extern frame::RegManager *reg_manager;

namespace live {

bool MoveList::Contain(INodePtr src, INodePtr dst) {
  return std::any_of(move_list_.cbegin(), move_list_.cend(),
                     [src, dst](std::pair<INodePtr, INodePtr> move) {
                       return move.first == src && move.second == dst;
                     });
}

void MoveList::Delete(INodePtr src, INodePtr dst) {
  assert(src && dst);
  auto move_it = move_list_.begin();
  for (; move_it != move_list_.end(); move_it++) {
    if (move_it->first == src && move_it->second == dst) {
      break;
    }
  }
  move_list_.erase(move_it);
}

MoveList *MoveList::Union(MoveList *list) {
  auto *res = new MoveList();
  for (auto move : move_list_) {
    res->move_list_.push_back(move);
  }
  for (auto move : list->GetList()) {
    if (!res->Contain(move.first, move.second))
      res->move_list_.push_back(move);
  }
  return res;
}

MoveList *MoveList::Intersect(MoveList *list) {
  auto *res = new MoveList();
  for (auto move : list->GetList()) {
    if (Contain(move.first, move.second))
      res->move_list_.push_back(move);
  }
  return res;
}

temp::TempList* LiveGraphFactory::CalculateNewIn(temp::TempList* use, temp::TempList* old_out, temp::TempList* def) {
  auto res = new temp::TempList();
  std::unordered_set<temp::Temp*> result_set;

  // Step 1: Add `old_out - def` to `res`
  if (old_out) {
    auto old_out_list = old_out->GetList();
    std::unordered_set<temp::Temp*> def_set;

    if (def) {
      for (auto def_temp : def->GetList()) {
        def_set.insert(def_temp);
      }
    }

    for (auto old_temp : old_out_list) {
      if (def_set.find(old_temp) == def_set.end()) {
        res->Append(old_temp);
        result_set.insert(old_temp);
      }
    }
  }

  // Step 2: Add `use` to `res` if not already present
  if (use) {
    for (auto use_temp : use->GetList()) {
      if (result_set.find(use_temp) == result_set.end()) {
        res->Append(use_temp);
        result_set.insert(use_temp);
      }
    }
  }

  return res;
}
temp::TempList* LiveGraphFactory::CalculateNewOut(fg::FNodePtr n) {
    auto res = new temp::TempList();

    // Helper lambda to check if a Temp is already in a TempList
    auto contains = [](temp::TempList* list, temp::Temp* temp) -> bool {
        if (list == nullptr) return false;
        for (auto item : list->GetList()) {
            if (item == temp) {
                return true;
            }
        }
        return false;
    };

    // Iterate over successors
    for (auto succ_node : n->Succ()->GetList()) {
        auto succ_in = in_->Look(succ_node);
        if (succ_in == nullptr) {
            // Handle error case (e.g., assertion or logging)
            assert(false && "Successor node's IN set is null.");
            continue;
        }

        // Add unique elements from succ_in to res
        for (auto succ_temp : succ_in->GetList()) {
            if (!contains(res, succ_temp)) {
                res->Append(succ_temp);
            }
        }
    }

    return res;
}
void LiveGraphFactory::LiveMap() {
  for (const auto& instr_node : flowgraph_->Nodes()->GetList()) {
      in_->Enter(instr_node, new temp::TempList());
      out_->Enter(instr_node, new temp::TempList());
  }
  bool has_changes;
  do
  {
    has_changes = false;

    for (const auto& instr_node : flowgraph_->Nodes()->GetList()) {
      auto old_in = in_->Look(instr_node);
      auto old_out = out_->Look(instr_node);

      if (!old_in || !old_out) {
          assert(false && "Node's IN or OUT set is null.");
          return;
      }

      auto new_in = CalculateNewIn(instr_node->NodeInfo()->Use(), old_out, instr_node->NodeInfo()->Def());
      auto new_out = CalculateNewOut(instr_node);

      if (new_in->GetList().size() != old_in->GetList().size() ||
          new_out->GetList().size() != old_out->GetList().size()) {
          has_changes = true;
      }

      in_->Set(instr_node, new_in);
      out_->Set(instr_node, new_out);

      delete old_in;
      delete old_out;
    }
  } while (has_changes);
}

// 辅助函数：添加冲突边
void LiveGraphFactory::AddInterferenceEdge(temp::Temp* t1, temp::Temp* t2) {
  auto node1 = temp_node_map_->Look(t1);
  auto node2 = temp_node_map_->Look(t2);
  assert(node1 && node2 && "Node lookup failed.");
  live_graph_.interf_graph->AddEdge(node1, node2);
  live_graph_.interf_graph->AddEdge(node2, node1);
}

// 辅助函数：添加移动指令边
void LiveGraphFactory::AddMoveEdges(temp::Temp* def, temp::TempList* use_set) {
  for (auto use : use_set->GetList()) {
      if (use == reg_manager->Rsp()) {
          continue;
      }
      auto def_node = temp_node_map_->Look(def);
      auto use_node = temp_node_map_->Look(use);
      if (!live_graph_.moves->Contain(def_node, use_node) && 
          !live_graph_.moves->Contain(use_node, def_node)) {
          live_graph_.moves->Append(def_node, use_node);
      }
  }
}

void LiveGraphFactory::InterfGraph() {
  for (auto reg : reg_manager->Registers()->GetList()) {
    auto reg_node = live_graph_.interf_graph->NewNode(reg);
    temp_node_map_->Enter(reg, reg_node);
  }

  const auto& reg_list = reg_manager->Registers()->GetList();
  for (auto reg1 : reg_list) {
    for (auto reg2 : reg_list) {
      if (reg1 != reg2) {
        auto node1 = temp_node_map_->Look(reg1);
        auto node2 = temp_node_map_->Look(reg2);
        assert(node1 && node2 && "Precolored node lookup failed.");
        live_graph_.interf_graph->AddEdge(node1, node2);
      }
    }
  }

  for (auto instr_node : flowgraph_->Nodes()->GetList()) {
    auto out_set = out_->Look(instr_node);
    auto def_set = instr_node->NodeInfo()->Def();
    auto out_union_def = out_set->Union(def_set);

    for (auto temp : out_union_def->GetList()) {
        if (temp == reg_manager->Rsp()) {
            continue; // 不将 %rsp 加入图中
        }
        if (!temp_node_map_->Look(temp)) {
            auto temp_node = live_graph_.interf_graph->NewNode(temp);
            temp_node_map_->Enter(temp, temp_node);
        }
    }
    delete out_union_def;
  }

  for (auto instr_node : flowgraph_->Nodes()->GetList()) {
    auto instr_info = instr_node->NodeInfo();
    auto out_set = out_->Look(instr_node);

    if (typeid(*instr_info) == typeid(assem::MoveInstr)) {
      auto use_set = instr_info->Use();
      auto def_set = instr_info->Def();
      auto out_diff_use = out_set->Diff(use_set);

      for (auto def : def_set->GetList()) {
          if (def == reg_manager->Rsp()) {
              continue;
          }
          for (auto live : out_diff_use->GetList()) {
              if (live == reg_manager->Rsp()) {
                  continue;
              }
              AddInterferenceEdge(def, live);
          }
          AddMoveEdges(def, use_set);
      }
      delete out_diff_use;

    } else {

      auto def_set = instr_info->Def();
      if (def_set) {
        for (auto def : def_set->GetList()) {
          if (def == reg_manager->Rsp()) {
            continue;
          }
          for (auto out : out_set->GetList()) {
            if (out == reg_manager->Rsp()) {
              continue;
            }
            AddInterferenceEdge(def, out);
          }
        }
      }
    }
  }
}


void LiveGraphFactory::Liveness() {
  
  LiveMap();
  InterfGraph();
}

} // namespace live
