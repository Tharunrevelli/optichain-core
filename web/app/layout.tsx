import './globals.css';
import type { Metadata } from 'next';

export const metadata: Metadata = {
  title: 'OptiChain Decision Studio | Enterprise Multi-Echelon Supply Chain & Spare Parts Optimizer',
  description: 'Enterprise analytical asset for multi-echelon inventory optimization, stochastic (s,S) continuous review simulation, and warranty liability forecasting.',
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <body className="antialiased selection:bg-indigo-500 selection:text-white">{children}</body>
    </html>
  );
}
