'use client';

import React from 'react';
import { ArrowRight, CheckCircle2, FileSpreadsheet, GitCompare, Sparkles, TrendingUp } from 'lucide-react';

export default function BusinessCaseSummary() {
  const comparisonData = [
    {
      dimension: 'Supply Chain Topology',
      asIs: 'Siloed, decentralized regional buffers with uncoordinated replenishment',
      toBe: 'Synchronized 3-tier DAG multi-echelon continuous review (s, S) policy',
      impact: '24% reduction in systemic inventory holding cost',
    },
    {
      dimension: 'Spare Parts Allocation',
      asIs: 'Heuristic rule-of-thumb order quantities causing critical stockouts',
      toBe: 'Sherbrooke METRIC & Knapsack Dynamic Programming optimizer',
      impact: 'Service level elevated from 82.5% to 99.2% fleet availability',
    },
    {
      dimension: 'Warranty Operations',
      asIs: 'Reactive warranty payouts with high emergency shipment premiums',
      toBe: 'Weibull hazard degradation forecasting + telemetry pre-staging',
      impact: '$3.36M reduction in annual warranty reserve liabilities',
    },
    {
      dimension: 'Computation Engine',
      asIs: 'Legacy batch spreadsheets taking hours with high error rates',
      toBe: 'Modern C++ Core engine executing 365-day DES in 3.3 milliseconds',
      impact: 'Sub-second real-time what-if scenario exploration for executives',
    },
  ];

  return (
    <div className="bg-white rounded-xl border border-slate-200 shadow-sm p-6">
      <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4 pb-6 border-b border-slate-100">
        <div>
          <div className="flex items-center gap-2 text-indigo-600 font-semibold text-xs uppercase tracking-wider">
            <GitCompare className="w-4 h-4" /> Strategy & Operations Transformation
          </div>
          <h2 className="text-xl font-bold text-slate-900 mt-1">
            Enterprise Operating Model: Current State (As-Is) vs Future State (To-Be)
          </h2>
          <p className="text-xs text-slate-500 mt-0.5">
            Accenture S&C Business Value Case & Capability Architecture
          </p>
        </div>

        <div className="flex items-center gap-2">
          <span className="inline-flex items-center gap-1.5 px-3 py-1 rounded-full text-xs font-semibold bg-indigo-50 text-indigo-700 border border-indigo-200">
            <Sparkles className="w-3.5 h-3.5" /> Quantified Value Delivery
          </span>
        </div>
      </div>

      <div className="mt-6 overflow-x-auto">
        <table className="w-full text-left text-xs">
          <thead>
            <tr className="bg-slate-50 text-slate-500 uppercase tracking-wider border-b border-slate-200">
              <th className="py-3 px-4 font-semibold">Operational Dimension</th>
              <th className="py-3 px-4 font-semibold text-rose-700 bg-rose-50/50">Current State (As-Is)</th>
              <th className="py-3 px-4 font-semibold text-indigo-700 bg-indigo-50/50">Target State (To-Be)</th>
              <th className="py-3 px-4 font-semibold text-emerald-700 bg-emerald-50/50">Quantified Business Impact</th>
            </tr>
          </thead>
          <tbody className="divide-y divide-slate-100 text-slate-700">
            {comparisonData.map((row, idx) => (
              <tr key={idx} className="hover:bg-slate-50/50 transition-colors">
                <td className="py-3.5 px-4 font-bold text-slate-900">{row.dimension}</td>
                <td className="py-3.5 px-4 text-slate-600 bg-rose-50/20">{row.asIs}</td>
                <td className="py-3.5 px-4 font-medium text-slate-800 bg-indigo-50/20">{row.toBe}</td>
                <td className="py-3.5 px-4 font-bold text-emerald-600 bg-emerald-50/20">{row.impact}</td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-3 gap-4 mt-6 pt-6 border-t border-slate-200">
        <div className="bg-gradient-to-br from-indigo-50 to-slate-50 p-4 rounded-xl border border-indigo-100">
          <div className="flex items-center gap-2 text-indigo-700 font-bold text-xs uppercase">
            <TrendingUp className="w-4 h-4" /> Working Capital ROI
          </div>
          <p className="text-2xl font-black text-slate-900 mt-1">24.2% Savings</p>
          <p className="text-xs text-slate-500 mt-1">
            Optimized safety stock sizing frees up $1.8M in trapped multi-echelon inventory capital.
          </p>
        </div>

        <div className="bg-gradient-to-br from-emerald-50 to-slate-50 p-4 rounded-xl border border-emerald-100">
          <div className="flex items-center gap-2 text-emerald-700 font-bold text-xs uppercase">
            <CheckCircle2 className="w-4 h-4" /> SLA Resilience
          </div>
          <p className="text-2xl font-black text-slate-900 mt-1">99.2% Availability</p>
          <p className="text-xs text-slate-500 mt-1">
            Dynamic review eliminates stockout penalties across critical avionics and powertrain components.
          </p>
        </div>

        <div className="bg-gradient-to-br from-blue-50 to-slate-50 p-4 rounded-xl border border-blue-100">
          <div className="flex items-center gap-2 text-blue-700 font-bold text-xs uppercase">
            <FileSpreadsheet className="w-4 h-4" /> Computational Edge
          </div>
          <p className="text-2xl font-black text-slate-900 mt-1">3.3 ms Runtime</p>
          <p className="text-xs text-slate-500 mt-1">
            C++14 priority queue DES enables high-velocity Monte Carlo stress testing and real-time decision support.
          </p>
        </div>
      </div>
    </div>
  );
}
