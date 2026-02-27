// #!omni:code -typescript\--> module
// ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ [BLOCK:METADATA]                                                                                                   ║
// ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣
// ║                                                                                                                    ║
// ║ Key:         B-DSA-UTILS                                                                                           ║
// ║ Version:     a-01.00                                                                                                ║
// ║ Purpose:     Universal CLI utilities for logging and user interaction.                                              ║
// ║                                                                                                                    ║
// ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

const LOG_DIR = "sdk/algorithm/fixtures/logs";
let logBuffer: string[] = [];
let logFilePath: string;

const originalConsoleLog = console.log;

export async function initLogging() {
    await Deno.mkdir(LOG_DIR, { recursive: true });
    const timestamp = new Date().toISOString().replace(/[:.]/g, "-");
    logFilePath = `${LOG_DIR}/dsa-run-${timestamp}.log`;
    logBuffer.push(`--- DSA REPL Session Log: ${timestamp} ---\n`);

    console.log = (...args: any[]) => {
        const message = args.map(arg => String(arg)).join(" ");
        logBuffer.push(message);
        originalConsoleLog(...args);
    };

    console.log(`Logging all output to: ${logFilePath}`);
}

export async function writeLogFile() {
    try {
        await Deno.writeTextFile(logFilePath, logBuffer.join("\n"));
        originalConsoleLog(`Session log saved to: ${logFilePath}`);
    } catch (error) {
        originalConsoleLog(`Error saving log file: ${error.message}`);
    }
}

export async function prompt(question: string): Promise<string> {
    logBuffer.push(`PROMPT: ${question}`);
    const buf = new Uint8Array(1024);
    originalConsoleLog(question);
    const n = await Deno.stdin.read(buf);
    const answer = new TextDecoder().decode(buf.subarray(0, n)).trim();
    logBuffer.push(`ANSWER: ${answer}`);
    return answer;
}
