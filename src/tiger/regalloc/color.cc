#include "tiger/regalloc/color.h"
#include "tiger/frame/temp.h"
#include <iostream>

extern frame::RegManager *reg_manager;

namespace col {
/* TODO: Put your lab6 code here */

col::Result Color::RAColor(){
    Build();
    MakeWorklist();
    while(!(simplifyWorklist->GetList().empty() && worklistMoves->GetList().empty() && freezeWorklist->GetList().empty() && spillWorklist->GetList().empty())){
        if(!simplifyWorklist->GetList().empty()){
            Simplify();
        }else if(!worklistMoves->GetList().empty()){
            Coalesce();
        }else if(!freezeWorklist->GetList().empty()){
            Freeze();
        }else if(!spillWorklist->GetList().empty()){
            SelectSpill();
        }
    }
    AssignColors();
    if(!spilledNodes->GetList().empty()){
        auto res = col::Result();
        res.spills = spilledNodes;
        return res;
    }else{
        auto res = col::Result();
        res.coloring = temp::Map::Empty();
        auto node_list = interference_graph->Nodes()->GetList();
        for(auto node : node_list){
            auto temp = node->NodeInfo();
            auto node_color = color->Look(node);
            auto color_str = reg_manager->temp_map_->Look(node_color);
            res.coloring->Enter(node->NodeInfo(), color_str);
        }
        return res;
    }
}

void Color::Build(){
    flow_fac = new fg::FlowGraphFactory(instr_list);
    flow_fac->AssemFlowGraph();
    live_fac = new live::LiveGraphFactory(flow_fac->GetFlowGraph());
    live_fac->Liveness();
    auto live_graph = live_fac->GetLiveGraph();
    interference_graph = live_graph.interf_graph;
    precolored = new live::INodeList();
    simplifyWorklist = new live::INodeList();
    freezeWorklist = new live::INodeList();
    spillWorklist = new live::INodeList();
    spilledNodes = new live::INodeList();
    coalescedNodes = new live::INodeList();
    coloredNodes = new live::INodeList();
    selectStack = new live::INodeList();

    coalescedMoves = new live::MoveList();
    constrainedMoves = new live::MoveList();
    frozenMoves = new live::MoveList();
    worklistMoves = live_graph.moves;
    activeMoves = new live::MoveList();
    
    
    degree = new graph::Table<temp::Temp, int>();
    moveList = new graph::Table<temp::Temp, live::MoveList>();
    alias = new graph::Table<temp::Temp, live::INode>();
    color = new graph::Table<temp::Temp, temp::Temp>();

    auto machine_reg_list = reg_manager->Registers()->GetList();
    auto node_map = live_fac->GetTempNodeMap();
    for(auto reg : machine_reg_list){
        auto reg_node = node_map->Look(reg);
        precolored->Append(reg_node);
        coloredNodes->Append(reg_node);
        color->Enter(reg_node, reg);
    }

    auto node_list = interference_graph->Nodes()->GetList();
    for(auto node : node_list){

        auto current_node_move = new live::MoveList();
        auto all_moves = live_graph.moves->GetList();
        for(auto single_move : all_moves){
            if(single_move.first == node || single_move.second == node){
                if(!current_node_move->Contain(single_move.first, single_move.second)){
                    current_node_move->Append(single_move.first, single_move.second);
                }
            }
        }
        moveList->Enter(node, current_node_move);
        int *node_degree = new int(0);
        if(precolored->Contain(node)){
            // precolored nodes have very large degree
            *node_degree = 10000;
        }else{
            *node_degree = node->OutDegree();
        }
        degree->Enter(node, node_degree);
    }
}

void Color::AddEdge(live::INodePtr u, live::INodePtr v){
    if(!u->Succ()->Contain(v) && u != v){
        interference_graph->AddEdge(u, v);
        interference_graph->AddEdge(v, u);
        auto u_degree_ptr = degree->Look(u);
        auto v_degree_ptr = degree->Look(v);
        auto u_value = *u_degree_ptr;
        auto v_value = *v_degree_ptr;
        *u_degree_ptr = u_value + 1;
        *v_degree_ptr = v_value + 1;
    }
}

void Color::MakeWorklist(){
    auto node_list = interference_graph->Nodes()->GetList();
    for(auto node : node_list){
        if(!precolored->Contain(node)){
            if(*(degree->Look(node)) >= K){
                spillWorklist->Append(node);
            }
            else if(MoveRelated(node)){
                freezeWorklist->Append(node);
            }
            else{
                simplifyWorklist->Append(node);
            }
        }
    }
}

live::INodeListPtr Color::Adjacent(live::INodePtr n){
    auto adjList = n->Succ();
    auto select_union_coalesced = selectStack->Union(coalescedNodes);
    auto res = adjList->Diff(select_union_coalesced);

    return res;
}


live::MoveList* Color::NodeMoves(live::INodePtr n){
    auto moveList_n = moveList->Look(n);
    auto active_union_work = activeMoves->Union(worklistMoves);
    auto res = moveList_n->Intersect(active_union_work);

    return res;
}

bool Color::MoveRelated(live::INodePtr n){
    auto node_moves = NodeMoves(n);
    return !node_moves->GetList().empty();
}

void Color::Simplify(){
    auto n = simplifyWorklist->GetList().front();
    simplifyWorklist->DeleteNode(n);
    selectStack->Prepend(n);
    auto adjacent_node_list = Adjacent(n)->GetList();
    for(auto single_node : adjacent_node_list){
        DecrementDegree(single_node);
    }
}


void Color::DecrementDegree(live::INodePtr m){
    auto degree_ptr = degree->Look(m);
    auto origin_degree = *degree_ptr;
    *degree_ptr = origin_degree - 1;
    if(origin_degree == K){
        auto m_union_adj = Adjacent(m);
        m_union_adj->Append(m);
        EnableMoves(m_union_adj);
        spillWorklist->DeleteNode(m);
        if(MoveRelated(m)){
            freezeWorklist->Append(m);
        }else{
            simplifyWorklist->Append(m);
        }
    }
}


void Color::EnableMoves(live::INodeListPtr nodes){
    auto nodes_list = nodes->GetList();
    for(auto n : nodes_list){
        auto node_moves = NodeMoves(n);
        auto node_moves_list = node_moves->GetList();
        for(auto m : node_moves_list){
            if(activeMoves->Contain(m.first, m.second)){
                activeMoves->Delete(m.first, m.second);
                if(!worklistMoves->Contain(m.first, m.second)){
                    worklistMoves->Append(m.first, m.second);
                }
            }
        }
    }
}

void Color::Coalesce(){
    auto worklistMoves_list = worklistMoves->GetList();
    auto m = worklistMoves_list.front();
    auto x = GetAlias(m.first);
    auto y = GetAlias(m.second);
    live::INodePtr u = nullptr;
    live::INodePtr v = nullptr;
    if(precolored->Contain(y)){
        u = y;
        v = x;
    }else{
        u = x;
        v = y;
    }
    worklistMoves->Delete(m.first, m.second);
    if(u == v){
        if(!coalescedMoves->Contain(m.first, m.second)){
            coalescedMoves->Append(m.first, m.second);
        }
        AddWorkList(u);
    }else if(precolored->Contain(v) || u->Succ()->Contain(v)){
        if(!constrainedMoves->Contain(m.first, m.second)){
            constrainedMoves->Append(m.first, m.second);
        }
        AddWorkList(u);
        AddWorkList(v);
    }else if((precolored->Contain(u) && OK(v, u)) || (!precolored->Contain(u) && Conservative(Adjacent(u)->Union(Adjacent(v))))){
        if(!coalescedMoves->Contain(m.first, m.second)){
            coalescedMoves->Append(m.first, m.second);
        }
        Combine(u, v);
        AddWorkList(u);
    }else{
        if(!activeMoves->Contain(m.first, m.second)){
            activeMoves->Append(m.first, m.second);
        }
    }
}


void Color::AddWorkList(live::INodePtr u){
    if(!precolored->Contain(u) && !MoveRelated(u) && (*(degree->Look(u)) < K)){
        freezeWorklist->DeleteNode(u);
        if(!simplifyWorklist->Contain(u)){
            simplifyWorklist->Append(u);
        }
    }
}


bool Color::OK(live::INodePtr t, live::INodePtr r){
    auto adj_list = Adjacent(t)->GetList();
    for(auto adj : adj_list){
        if(!((*(degree->Look(adj)) < K) || (precolored->Contain(adj)) || (adj->Succ()->Contain(r)))){
            return false;
        }
    }
    return true;
}

bool Color::Conservative(live::INodeListPtr nodes){
    int k = 0;
    auto node_list = nodes->GetList();
    for(auto n : node_list){
        if(*(degree->Look(n)) >= K){
            k = k + 1;
        }
    }
    return k < K;
}

live::INodePtr Color::GetAlias(live::INodePtr n){
    if(coalescedNodes->Contain(n)){
        return GetAlias(alias->Look(n));
    }
    return n;
}

void Color::Combine(live::INodePtr u, live::INodePtr v){
    if(freezeWorklist->Contain(v)){
        freezeWorklist->DeleteNode(v);
    }
    else{
        spillWorklist->DeleteNode(v);
    }
    coalescedNodes->Append(v);
    alias->Enter(v, u);
    auto moveList_u = moveList->Look(u);
    auto moveList_v = moveList->Look(v);
    auto moveList_u_union_v = moveList_u->Union(moveList_v);

    moveList->Set(u, moveList_u_union_v);
    auto v_list = new live::INodeList();
    v_list->Append(v);
    EnableMoves(v_list);

    auto adjacent = Adjacent(v);
    auto adj_list = adjacent->GetList();
    for(auto t : adj_list){
        AddEdge(t, u);
        DecrementDegree(t);
    }
    if(*(degree->Look(u)) >= K && freezeWorklist->Contain(u)){
        freezeWorklist->DeleteNode(u);
        if(!spillWorklist->Contain(u)){
            spillWorklist->Append(u);
        }
    }
}


void Color::Freeze(){
    auto u = freezeWorklist->GetList().front();
    freezeWorklist->DeleteNode(u);
    if(!simplifyWorklist->Contain(u)){
        simplifyWorklist->Append(u);
    }
    FreezeMoves(u);
}

void Color::FreezeMoves(live::INodePtr u){
    auto node_moves = NodeMoves(u);
    auto node_moves_list = node_moves->GetList();
    for(auto m : node_moves_list){
        auto x = m.first;
        auto y = m.second;
        live::INodePtr v = nullptr;
        if(GetAlias(y) == GetAlias(u)){
            v = GetAlias(x);
        }
        else{
            v = GetAlias(y);
        }
        activeMoves->Delete(x, y);
        if(!frozenMoves->Contain(x, y)){
            frozenMoves->Append(x, y);
        }
        auto node_moves_v = NodeMoves(v);
        auto node_moves_list_v = node_moves_v->GetList();
        if(node_moves_list_v.empty() && *(degree->Look(v)) < K){
            freezeWorklist->DeleteNode(v);
            if(!simplifyWorklist->Contain(v)){
                simplifyWorklist->Append(v);
            }
        }
    }
}


void Color::SelectSpill(){
    auto m = spillWorklist->GetList().front();
    spillWorklist->DeleteNode(m);
    if(!simplifyWorklist->Contain(m)){
        simplifyWorklist->Append(m);
    }
    FreezeMoves(m);
}

void Color::AssignColors(){
    auto selectStack_list = selectStack->GetList();
    for(auto single_node : selectStack_list){
        auto ok_colors = reg_manager->Registers();
        auto adj_list = single_node->Succ()->GetList();
        auto colored_union_precolored = coloredNodes->Union(precolored);
        // conflict with all adjacent nodes' color
        for(auto w : adj_list){
            auto w_alias = GetAlias(w);
            if(colored_union_precolored->Contain(w_alias)){
                auto temp_colored = new temp::TempList();
                temp_colored->Append(color->Look(w_alias));
                ok_colors = ok_colors->Diff(temp_colored);
            }
        }
        if(ok_colors->GetList().empty()){
            if(!spilledNodes->Contain(single_node)){
                spilledNodes->Append(single_node);
            }
        }
        else{
            if(!coloredNodes->Contain(single_node)){
                coloredNodes->Append(single_node);
            }
            auto selected_color = ok_colors->GetList().front();
            color->Enter(single_node, selected_color);
        }
    }
    selectStack->Clear();
    auto coalesced_list = coalescedNodes->GetList();
    for(auto node : coalesced_list){
        auto alias_color = color->Look(GetAlias(node));
        color->Enter(node, alias_color);
    }
}




} // namespace col