'use client';

import React, { useState } from 'react';
import { ShieldAlert, DollarSign, Activity, FileText, CheckCircle2 } from 'lucide-react';
import { BarChart, Bar, XAxis, YAxis, Tooltip, ResponsiveContainer, CartesianGrid } from 'recharts';

export default function WarrantyCalculator() {
  const [installedBase, setInstalledBase] = useState<number>(25000);
  const [warrantyMonths, setWarrantyMonths] = useState<number>(36);
  const [shapeBeta, setShapeBeta] = useState<number>(1.45);
  const [scaleEta, setScaleEta] = useState<number>(68);
  const [avgClaimCost, setAvgClaimCost] = useState<number>(1450);

  // Weibull calculation
  const calculateWeibull = () => {
    const monthlyData = [];
    let cumulativeClaims = 0;
    let prevCDF = 0;

    for (let m = 3; m <= warrantyMonths; m += 3) {
      const cdf = 1 - Math.exp(-Math.pow(m / scaleEta, shapeBeta));
      const periodP = Math.max(0, cdf - prevCDF);
      prevCDF = cdf;
      const claims = Math.round(installedBase * periodP);
      cumulativeClaims += claims;
      const cost = claims * avgClaimCost;

      monthlyData.push({
        month: `M${m}`,
        claims,
        cost: Math.round(cost / 1000), // in $k
      });
    }

    const totalReserve = cumulativeClaims * avgClaimCost;
    // Proactive refurbishment intercepts 35% of catastrophic claims and fixes them at $280 instead of $1450
    const intercepted = cumulativeClaims * 0.35;
    const proactiveSavings = intercepted * (avgClaimCost - 280);

    return {
      monthlyData,
      cumulativeClaims,
      totalReserve,
      proactiveSavings,
    };
  };

  const { monthlyData, cumulativeClaims, totalReserve, proactiveSavings } = calculateWeibull();

  return (
    <div className="bg-white rounded-xl border border-slate-200 shadow-sm p-6">
      <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4 pb-6 border-b border-slate-100">
        <div>
          <div className="flex items-center gap-2 text-indigo-600 font-semibold text-xs uppercase tracking-wider">
            <ShieldAlert className="w-4 h-4" /> After-Market Service Operations
          </div>
          <h2 className="text-xl font-bold text-slate-900 mt-1">
            Weibull Warranty Degradation & Reserve Liabilities Forecaster
          </h2>
          <p className="text-xs text-slate-500 mt-0.5">
            Predictive reserve planning & preventative field refurbishment ROI model
          </p>
        </div>

        <div className="flex items-center gap-2">
          <span className="inline-flex items-center gap-1.5 px-3 py-1 rounded-full text-xs font-semibold bg-emerald-50 text-emerald-700 border border-emerald-200">
            <CheckCircle2 className="w-3.5 h-3.5" /> 28.2% Reserve Reduction
          </span>
        </div>
      </div>

      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6 mt-6">
        {/* Sliders & Parameters */}
        <div className="bg-slate-50/70 rounded-xl p-5 border border-slate-200 space-y-4">
          <h4 className="text-xs font-bold text-slate-700 uppercase tracking-wider">Fleet & Failure Parameters</h4>

          <div>
            <div className="flex justify-between text-xs font-medium text-slate-700 mb-1">
              <span>Installed Base:</span>
              <span className="font-bold text-indigo-600">{installedBase.toLocaleString()} units</span>
            </div>
            <input
              type="range"
              min="5000"
              max="50000"
              step="2500"
              value={installedBase}
              onChange={(e) => setInstalledBase(Number(e.target.value))}
              className="w-full h-1.5 bg-slate-200 rounded-lg appearance-none cursor-pointer accent-indigo-600"
            />
          </div>

          <div>
            <div className="flex justify-between text-xs font-medium text-slate-700 mb-1">
              <span>Warranty Horizon:</span>
              <span className="font-bold text-indigo-600">{warrantyMonths} months</span>
            </div>
            <input
              type="range"
              min="12"
              max="60"
              step="6"
              value={warrantyMonths}
              onChange={(e) => setWarrantyMonths(Number(e.target.value))}
              className="w-full h-1.5 bg-slate-200 rounded-lg appearance-none cursor-pointer accent-indigo-600"
            />
          </div>

          <div>
            <div className="flex justify-between text-xs font-medium text-slate-700 mb-1">
              <span>Weibull Shape (β):</span>
              <span className="font-bold text-indigo-600">{shapeBeta} (Wear-Out)</span>
            </div>
            <input
              type="range"
              min="0.8"
              max="2.5"
              step="0.05"
              value={shapeBeta}
              onChange={(e) => setShapeBeta(Number(e.target.value))}
              className="w-full h-1.5 bg-slate-200 rounded-lg appearance-none cursor-pointer accent-indigo-600"
            />
            <span className="text-[10px] text-slate-400">β &gt; 1 indicates aging wear-out hazard rate</span>
          </div>

          <div>
            <div className="flex justify-between text-xs font-medium text-slate-700 mb-1">
              <span>Avg Unmitigated Claim Cost:</span>
              <span className="font-bold text-rose-600">${avgClaimCost.toLocaleString()}</span>
            </div>
            <input
              type="range"
              min="500"
              max="3000"
              step="100"
              value={avgClaimCost}
              onChange={(e) => setAvgClaimCost(Number(e.target.value))}
              className="w-full h-1.5 bg-slate-200 rounded-lg appearance-none cursor-pointer accent-indigo-600"
            />
          </div>

          <div className="pt-3 border-t border-slate-200 text-xs text-slate-500">
            <p className="font-medium text-slate-700">Preventative Inspection Cost: $280/unit</p>
            <p className="text-[11px] mt-0.5">IoT sensors & MES telemetry trigger field repair before catastrophic breakdown.</p>
          </div>
        </div>

        {/* Projections Chart */}
        <div className="lg:col-span-2 bg-slate-50/70 rounded-xl p-5 border border-slate-200 flex flex-col justify-between">
          <div>
            <div className="flex justify-between items-center mb-4">
              <h4 className="text-sm font-bold text-slate-800 flex items-center gap-2">
                <Activity className="w-4 h-4 text-indigo-600" /> Quarterly Warranty Incident Projections & Costs ($k)
              </h4>
              <span className="text-xs text-slate-500">Quarterly intervals</span>
            </div>

            <div className="h-56 w-full">
              <ResponsiveContainer width="100%" height="100%">
                <BarChart data={monthlyData} margin={{ top: 10, right: 10, left: 0, bottom: 0 }}>
                  <CartesianGrid strokeDasharray="3 3" vertical={false} stroke="#e2e8f0" />
                  <XAxis dataKey="month" tick={{ fontSize: 11 }} />
                  <YAxis unit="k$" tick={{ fontSize: 11 }} />
                  <Tooltip
                    formatter={(val: any) => [`$${val},000 USD`, 'Claim Reserve']}
                    labelFormatter={(lbl) => `Warranty Timeline: ${lbl}`}
                  />
                  <Bar dataKey="cost" fill="#6366f1" radius={[4, 4, 0, 0]} />
                </BarChart>
              </ResponsiveContainer>
            </div>
          </div>

          {/* Metric cards */}
          <div className="grid grid-cols-1 sm:grid-cols-3 gap-3 mt-4 pt-4 border-t border-slate-200">
            <div className="bg-white p-3 rounded-lg border border-slate-200">
              <span className="text-[10px] text-slate-400 font-bold uppercase">Total Claims Expected</span>
              <p className="text-lg font-bold text-slate-800 mt-0.5">{cumulativeClaims.toLocaleString()} incidents</p>
            </div>
            <div className="bg-white p-3 rounded-lg border border-slate-200">
              <span className="text-[10px] text-slate-400 font-bold uppercase">Unoptimized Reserve</span>
              <p className="text-lg font-bold text-rose-600 mt-0.5">${(totalReserve / 1e6).toFixed(2)}M USD</p>
            </div>
            <div className="bg-white p-3 rounded-lg border border-emerald-200 bg-emerald-50/30">
              <span className="text-[10px] text-emerald-700 font-bold uppercase">Proactive Net Savings</span>
              <p className="text-lg font-bold text-emerald-600 mt-0.5">${(proactiveSavings / 1e6).toFixed(2)}M USD</p>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
