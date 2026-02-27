// #!omni:code -typescript\--> module
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:METADATA]                                                                                                   ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣
// ║                                                                                                                    ║
// ║ Key:         B-DSA-TEST-RUNNER                                                                                     ║
// ║ Version:     a-01.00                                                                                                ║
// ║ Purpose:     Custom validation engine for structural and geometric integrity.                                        ║
// ║                                                                                                                    ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:SETUP]                                                                                                      ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣

import { DsaRegistry } from "../L0-foundation/registry.ts";
import { VaultLayout, ShalemReport } from "./engine.ts";

export interface TestResult {
    name: string;
    passed: boolean;
    error?: string;
    metrics?: Record<string, any>;
}

export type TestFn = () => Promise<void> | void;

// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:BODY]                                                                                                       ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣

export class CwsTestRunner {
    private tests: Map<string, TestFn> = new Map();
    private results: TestResult[] = [];

    /**
     * Registers a new structural test.
     */
    public test(name: string, fn: TestFn) {
        this.tests.set(name, fn);
    }

    /**
     * Assertion: Validates that a layout is "Shalem" (Sound).
     */
    public expectShalem(report: ShalemReport, context: string = "Layout Integrity") {
        if (!report.isShalem) {
            throw new Error(`[${context}] Not Shalem: ${report.violations.join(", ")}`);
        }
    }

    /**
     * Assertion: Validates the 3:2:1 ratio of a layout.
     */
    public expectRatio(layout: VaultLayout) {
        const expectedBreath = DsaRegistry.Breath;
        if (layout.functionalBreath !== expectedBreath) {
            throw new Error(`Ratio Deviation: Expected Breath ${expectedBreath}, found ${layout.functionalBreath}`);
        }
    }

    /**
     * Executes all registered tests and returns the results.
     */
    public async run() {
        console.log("\n--- [CTR] CWS Test Runner: Initializing Validation Loop ---");
        
        for (const [name, fn] of this.tests) {
            try {
                await fn();
                this.results.push({ name, passed: true });
            } catch (err) {
                this.results.push({ name, passed: false, error: err.message });
            }
        }
        
        this.report();
    }

    /**
     * Renders a structural report of the test results.
     */
    private report() {
        const total = this.results.length;
        const passed = this.results.filter(r => r.passed).length;
        const failed = total - passed;

        console.log("\n╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗");
        console.log(`║ [BLOCK:VALDIATION] CTR Test Results Summary                                                                        ║`);
        console.log("╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣");
        
        this.results.forEach(res => {
            const status = res.passed ? " [TOV] " : " [CHATA]";
            const line = `║ ${status} ${res.name.padEnd(60)} ║`;
            console.log(line);
            if (res.error) {
                console.log(`║         ! ${res.error.padEnd(106)} ║`);
            }
        });

        console.log("╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣");
        console.log(`║ TOTAL: ${total.toString().padEnd(10)} | PASSED: ${passed.toString().padEnd(10)} | FAILED: ${failed.toString().padEnd(10)} | HEALTH: ${((passed / total) * 100).toFixed(0)}% ║`);
        console.log("╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝");
    }
}

// Global instance for wiring up
export const CTR = new CwsTestRunner();

// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:CLOSING]                                                                                                    ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
