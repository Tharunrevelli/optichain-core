'use client';

import React from 'react';
import NetworkGraph from '../components/NetworkGraph';
import OptimizationStudio from '../components/OptimizationStudio';
import WarrantyCalculator from '../components/WarrantyCalculator';
import BusinessCaseSummary from '../components/BusinessCaseSummary';
import { Layers, Activity, ShieldAlert, GitCompare, Github, ExternalLink, Terminal } from 'lucide-react';

export default function Home() {
  return (
    <main className="min-h-screen bg-slate-100/70 py-10 px-4 sm:px-6 lg:px-8 max-w-7xl mx-auto space-y-8 font-sans">
      {/* Top Header */}
      <header className="bg-white rounded-2xl border border-slate-200 shadow-sm p-6 sm:p-8 flex flex-col md:flex-row md:items-center justify-between gap-6">
        <div>
          <div className="flex items-center gap-2">
            <span className="px-2.5 py-0.5 rounded-full text-xs font-bold uppercase tracking-wider bg-indigo-50 text-indigo-700 border border-indigo-200">
              Accenture S&C Asset
            </span>
            <span className="px-2.5 py-0.5 rounded-full text-xs font-bold uppercase tracking-wider bg-slate-100 text-slate-600">
              C++ Core + Next.js
            </span>
          </div>
          <h1 className="text-2xl sm:text-3xl font-extrabold text-slate-900 mt-2 tracking-tight">
            OptiChain Decision Studio
          </h1>
          <p className="text-sm text-slate-600 mt-1 max-w-3xl">
            High-Performance Multi-Echelon Supply Chain & Spare Parts Optimization Platform with C++ Discrete-Event Simulation, Constrained Knapsack Allocation, and Warranty Reserve Liabilities Forecasting.
          </p>
        </div>

        <div className="flex flex-wrap items-center gap-3 shrink-0">
          <div className="flex items-center gap-2 px-3 py-2 bg-slate-50 rounded-xl border border-slate-200 text-xs font-mono text-slate-700">
            <Terminal className="w-4 h-4 text-indigo-600" />
            <span>g++ -std=c++14 -O3 (3.3ms DES)</span>
          </div>
        </div>
      </header>

      {/* KPI Quick Cards */}
      <section className="grid grid-cols-2 md:grid-cols-4 gap-4">
        <div className="bg-white p-4 rounded-xl border border-slate-200 shadow-sm">
          <span className="text-[11px] font-bold text-slate-400 uppercase tracking-wider">Multi-Tier Network</span>
          <p className="text-2xl font-black text-slate-900 mt-0.5">3-Tier DAG</p>
          <span className="text-[11px] text-emerald-600 font-semibold mt-1 flex items-center gap-1">
            Central DC → RDC → Dealerships
          </span>
        </div>

        <div className="bg-white p-4 rounded-xl border border-slate-200 shadow-sm">
          <span className="text-[11px] font-bold text-slate-400 uppercase tracking-wider">Spare Parts Availability</span>
          <p className="text-2xl font-black text-indigo-600 mt-0.5">99.2% SLA</p>
          <span className="text-[11px] text-slate-500 font-medium mt-1">
            Sherbrooke METRIC & Knapsack DP
          </span>
        </div>

        <div className="bg-white p-4 rounded-xl border border-slate-200 shadow-sm">
          <span className="text-[11px] font-bold text-slate-400 uppercase tracking-wider">Working Capital ROI</span>
          <p className="text-2xl font-black text-emerald-600 mt-0.5">-$1.80M</p>
          <span className="text-[11px] text-emerald-700 font-medium mt-1">
            Trapped Safety Stock Released
          </span>
        </div>

        <div className="bg-white p-4 rounded-xl border border-slate-200 shadow-sm">
          <span className="text-[11px] font-bold text-slate-400 uppercase tracking-wider">Warranty Reserve Savings</span>
          <p className="text-2xl font-black text-indigo-600 mt-0.5">-$3.36M</p>
          <span className="text-[11px] text-slate-500 font-medium mt-1">
            28.2% Proactive Refurbishment Cut
          </span>
        </div>
      </section>

      {/* Section 1: Multi-Echelon DAG Topology */}
      <section>
        <NetworkGraph />
      </section>

      {/* Section 2: Spare Parts Optimization Studio */}
      <section>
        <OptimizationStudio />
      </section>

      {/* Section 3: Warranty Reserve Liabilities Forecaster */}
      <section>
        <WarrantyCalculator />
      </section>

      {/* Section 4: Strategy & Consulting Business Case (As-Is vs To-Be) */}
      <section>
        <BusinessCaseSummary />
      </section>

      {/* Footer */}
      <footer className="text-center py-6 text-xs text-slate-400 border-t border-slate-200">
        OptiChain Decision Studio • Enterprise Supply Chain & After-Market Strategy Platform • Built with Modern C++ & Next.js
      </footer>
    </main>
  );
}
