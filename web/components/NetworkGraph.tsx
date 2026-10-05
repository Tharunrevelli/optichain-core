'use client';

import React, { useState } from 'react';
import { Layers, MapPin, Truck, AlertTriangle, CheckCircle, ShieldCheck } from 'lucide-react';

export interface NetworkNode {
  id: number;
  name: string;
  location: string;
  tier: 'Central DC (Tier 0)' | 'Regional DC (Tier 1)' | 'Local Hub (Tier 2)';
  parentId: number | null;
  leadTimeDays: number;
  holdingCost: number;
  backorderPenalty: number;
  currentStock: number;
  targetFillRate: number;
  reorderPoint: number;
  orderUpTo: number;
}

const initialNodes: NetworkNode[] = [
  {
    id: 0,
    name: 'Central Distribution Center',
    location: 'Chicago, IL (Global Hub)',
    tier: 'Central DC (Tier 0)',
    parentId: null,
    leadTimeDays: 7.0,
    holdingCost: 0.25,
    backorderPenalty: 120,
    currentStock: 220,
    targetFillRate: 99.2,
    reorderPoint: 70,
    orderUpTo: 249,
  },
  {
    id: 1,
    name: 'East Coast RDC',
    location: 'Allentown, PA',
    tier: 'Regional DC (Tier 1)',
    parentId: 0,
    leadTimeDays: 2.5,
    holdingCost: 0.35,
    backorderPenalty: 140,
    currentStock: 95,
    targetFillRate: 98.4,
    reorderPoint: 29,
    orderUpTo: 112,
  },
  {
    id: 2,
    name: 'West Coast RDC',
    location: 'Reno, NV',
    tier: 'Regional DC (Tier 1)',
    parentId: 0,
    leadTimeDays: 3.0,
    holdingCost: 0.38,
    backorderPenalty: 140,
    currentStock: 104,
    targetFillRate: 98.1,
    reorderPoint: 33,
    orderUpTo: 115,
  },
  {
    id: 3,
    name: 'New York Metro Service Hub',
    location: 'Queens, NY',
    tier: 'Local Hub (Tier 2)',
    parentId: 1,
    leadTimeDays: 1.0,
    holdingCost: 0.55,
    backorderPenalty: 250,
    currentStock: 28,
    targetFillRate: 97.6,
    reorderPoint: 8,
    orderUpTo: 35,
  },
  {
    id: 4,
    name: 'Boston Industrial Hub',
    location: 'Waltham, MA',
    tier: 'Local Hub (Tier 2)',
    parentId: 1,
    leadTimeDays: 1.2,
    holdingCost: 0.50,
    backorderPenalty: 240,
    currentStock: 31,
    targetFillRate: 96.9,
    reorderPoint: 9,
    orderUpTo: 36,
  },
  {
    id: 5,
    name: 'SF Bay Area Hub',
    location: 'Oakland, CA',
    tier: 'Local Hub (Tier 2)',
    parentId: 2,
    leadTimeDays: 1.0,
    holdingCost: 0.60,
    backorderPenalty: 280,
    currentStock: 29,
    targetFillRate: 97.2,
    reorderPoint: 8,
    orderUpTo: 35,
  },
  {
    id: 6,
    name: 'LA Basin Service Hub',
    location: 'Long Beach, CA',
    tier: 'Local Hub (Tier 2)',
    parentId: 2,
    leadTimeDays: 1.1,
    holdingCost: 0.58,
    backorderPenalty: 270,
    currentStock: 30,
    targetFillRate: 97.8,
    reorderPoint: 8,
    orderUpTo: 34,
  },
];

export default function NetworkGraph() {
  const [selectedNode, setSelectedNode] = useState<NetworkNode>(initialNodes[0]);

  const rootNode = initialNodes[0];
  const tier1Nodes = initialNodes.filter((n) => n.tier === 'Regional DC (Tier 1)');
  const tier2Nodes = initialNodes.filter((n) => n.tier === 'Local Hub (Tier 2)');

  return (
    <div className="bg-white rounded-xl border border-slate-200 shadow-sm p-6">
      <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4 pb-6 border-b border-slate-100">
        <div>
          <div className="flex items-center gap-2 text-indigo-600 font-semibold text-xs uppercase tracking-wider">
            <Layers className="w-4 h-4" /> Multi-Echelon DAG Topology
          </div>
          <h2 className="text-xl font-bold text-slate-900 mt-1">
            Enterprise Logistics & Spare Parts Echelon Architecture
          </h2>
          <p className="text-xs text-slate-500 mt-0.5">
            Hierarchical inventory buffering model with dynamic cross-echelon stochastic replenishment
          </p>
        </div>
        <div className="flex items-center gap-2">
          <span className="inline-flex items-center gap-1.5 px-3 py-1 rounded-full text-xs font-medium bg-emerald-50 text-emerald-700 border border-emerald-200">
            <CheckCircle className="w-3.5 h-3.5" /> 7 Active Nodes Connected
          </span>
          <span className="inline-flex items-center gap-1.5 px-3 py-1 rounded-full text-xs font-medium bg-indigo-50 text-indigo-700 border border-indigo-200">
            <ShieldCheck className="w-3.5 h-3.5" /> C++ Core Synchronized
          </span>
        </div>
      </div>

      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6 mt-6">
        {/* Topology Visualization Canvas */}
        <div className="lg:col-span-2 bg-slate-50/70 rounded-xl p-5 border border-slate-200/80">
          <div className="space-y-6">
            {/* Level 0: Central DC */}
            <div className="flex flex-col items-center">
              <span className="text-[11px] font-bold text-slate-400 uppercase tracking-wider mb-2">
                Tier 0: Central Hub
              </span>
              <button
                onClick={() => setSelectedNode(rootNode)}
                className={`w-full max-w-md p-4 rounded-xl border text-left transition-all shadow-sm ${
                  selectedNode.id === rootNode.id
                    ? 'border-indigo-600 bg-white ring-2 ring-indigo-500/20'
                    : 'border-slate-200 bg-white hover:border-slate-300'
                }`}
              >
                <div className="flex items-center justify-between">
                  <div className="flex items-center gap-2.5">
                    <div className="p-2 bg-indigo-50 rounded-lg text-indigo-600 font-bold">
                      <MapPin className="w-4 h-4" />
                    </div>
                    <div>
                      <h4 className="text-sm font-bold text-slate-800">{rootNode.name}</h4>
                      <p className="text-xs text-slate-500">{rootNode.location}</p>
                    </div>
                  </div>
                  <div className="text-right">
                    <span className="text-xs font-bold text-indigo-600">
                      Fill: {rootNode.targetFillRate}%
                    </span>
                    <p className="text-[11px] text-slate-400">ROP: {rootNode.reorderPoint} | Cap: {rootNode.orderUpTo}</p>
                  </div>
                </div>
              </button>
            </div>

            {/* Connecting lines */}
            <div className="flex justify-around px-12 text-slate-300">
              <div className="h-4 border-l-2 border-dashed border-indigo-300"></div>
              <div className="h-4 border-l-2 border-dashed border-indigo-300"></div>
            </div>

            {/* Level 1: Regional DCs */}
            <div>
              <div className="text-center text-[11px] font-bold text-slate-400 uppercase tracking-wider mb-2">
                Tier 1: Regional Distribution Centers (RDCs)
              </div>
              <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
                {tier1Nodes.map((node) => (
                  <button
                    key={node.id}
                    onClick={() => setSelectedNode(node)}
                    className={`p-4 rounded-xl border text-left transition-all shadow-sm ${
                      selectedNode.id === node.id
                        ? 'border-indigo-600 bg-white ring-2 ring-indigo-500/20'
                        : 'border-slate-200 bg-white hover:border-slate-300'
                    }`}
                  >
                    <div className="flex items-center justify-between">
                      <div>
                        <span className="text-[10px] font-semibold uppercase tracking-wider px-2 py-0.5 rounded bg-blue-50 text-blue-700">
                          Transit: {node.leadTimeDays}d
                        </span>
                        <h4 className="text-sm font-bold text-slate-800 mt-1">{node.name}</h4>
                        <p className="text-xs text-slate-500">{node.location}</p>
                      </div>
                      <div className="text-right">
                        <span className="text-xs font-bold text-blue-600">
                          {node.targetFillRate}%
                        </span>
                        <p className="text-[11px] text-slate-400">ROP: {node.reorderPoint}</p>
                      </div>
                    </div>
                  </button>
                ))}
              </div>
            </div>

            {/* Connecting lines */}
            <div className="flex justify-around px-8 text-slate-300">
              <div className="h-4 border-l-2 border-dashed border-blue-300"></div>
              <div className="h-4 border-l-2 border-dashed border-blue-300"></div>
              <div className="h-4 border-l-2 border-dashed border-blue-300"></div>
              <div className="h-4 border-l-2 border-dashed border-blue-300"></div>
            </div>

            {/* Level 2: Local Hubs */}
            <div>
              <div className="text-center text-[11px] font-bold text-slate-400 uppercase tracking-wider mb-2">
                Tier 2: Field Service Hubs & Dealerships (Customer Facing)
              </div>
              <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-3">
                {tier2Nodes.map((node) => (
                  <button
                    key={node.id}
                    onClick={() => setSelectedNode(node)}
                    className={`p-3 rounded-lg border text-left transition-all ${
                      selectedNode.id === node.id
                        ? 'border-indigo-600 bg-white ring-2 ring-indigo-500/20 shadow'
                        : 'border-slate-200 bg-white hover:border-slate-300 shadow-sm'
                    }`}
                  >
                    <span className="text-[9px] font-semibold uppercase px-1.5 py-0.5 rounded bg-amber-50 text-amber-700">
                      Lead: {node.leadTimeDays}d
                    </span>
                    <h5 className="text-xs font-bold text-slate-800 mt-1 truncate">{node.name}</h5>
                    <p className="text-[11px] text-slate-400 truncate">{node.location}</p>
                    <div className="flex items-center justify-between mt-2 pt-2 border-t border-slate-100">
                      <span className="text-[10px] text-slate-500">Stock: {node.currentStock}</span>
                      <span className="text-[11px] font-semibold text-emerald-600">
                        {node.targetFillRate}%
                      </span>
                    </div>
                  </button>
                ))}
              </div>
            </div>
          </div>
        </div>

        {/* Selected Node Deep Dive Panel */}
        <div className="bg-slate-50/50 rounded-xl p-5 border border-slate-200 flex flex-col justify-between">
          <div>
            <div className="flex items-center justify-between pb-3 border-b border-slate-200">
              <span className="text-xs font-bold uppercase tracking-wider text-slate-500">
                Echelon Telemetry
              </span>
              <span className="px-2 py-0.5 rounded text-[11px] font-medium bg-indigo-100 text-indigo-700">
                Node #{selectedNode.id}
              </span>
            </div>

            <div className="mt-4">
              <h3 className="text-base font-bold text-slate-900">{selectedNode.name}</h3>
              <p className="text-xs text-slate-500 flex items-center gap-1 mt-0.5">
                <MapPin className="w-3.5 h-3.5 text-slate-400" /> {selectedNode.location}
              </p>
              <div className="inline-block mt-2 px-2.5 py-1 rounded bg-slate-200/70 text-slate-700 text-xs font-medium">
                {selectedNode.tier}
              </div>
            </div>

            <div className="grid grid-cols-2 gap-3 mt-5">
              <div className="bg-white p-3 rounded-lg border border-slate-200 shadow-sm">
                <span className="text-[11px] text-slate-400 font-medium">Reorder Point (s)</span>
                <p className="text-lg font-bold text-slate-800 mt-0.5">{selectedNode.reorderPoint} units</p>
              </div>
              <div className="bg-white p-3 rounded-lg border border-slate-200 shadow-sm">
                <span className="text-[11px] text-slate-400 font-medium">Order-Up-To (S)</span>
                <p className="text-lg font-bold text-indigo-600 mt-0.5">{selectedNode.orderUpTo} units</p>
              </div>
              <div className="bg-white p-3 rounded-lg border border-slate-200 shadow-sm">
                <span className="text-[11px] text-slate-400 font-medium">Lead Time</span>
                <p className="text-lg font-bold text-slate-800 mt-0.5">{selectedNode.leadTimeDays} days</p>
              </div>
              <div className="bg-white p-3 rounded-lg border border-slate-200 shadow-sm">
                <span className="text-[11px] text-slate-400 font-medium">Target Fill Rate</span>
                <p className="text-lg font-bold text-emerald-600 mt-0.5">{selectedNode.targetFillRate}%</p>
              </div>
            </div>

            <div className="mt-4 space-y-2 text-xs">
              <div className="flex justify-between py-1.5 border-b border-slate-200/60 text-slate-600">
                <span>Daily Holding Cost:</span>
                <span className="font-semibold text-slate-800">${selectedNode.holdingCost.toFixed(2)}/unit/day</span>
              </div>
              <div className="flex justify-between py-1.5 border-b border-slate-200/60 text-slate-600">
                <span>Stockout Penalty:</span>
                <span className="font-semibold text-rose-600">${selectedNode.backorderPenalty}/unit</span>
              </div>
              <div className="flex justify-between py-1.5 text-slate-600">
                <span>Current Pipeline Buffer:</span>
                <span className="font-semibold text-slate-800">{selectedNode.currentStock} units on-hand</span>
              </div>
            </div>
          </div>

          <div className="mt-6 pt-4 border-t border-slate-200">
            <div className="flex items-center gap-2 text-xs text-slate-600 bg-amber-50 p-2.5 rounded-lg border border-amber-200">
              <AlertTriangle className="w-4 h-4 text-amber-600 shrink-0" />
              <span>Continuous review (s, S) policy ensures 98%+ SLA while minimizing safety stock working capital.</span>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
