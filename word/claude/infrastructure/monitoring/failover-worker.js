// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-failover-worker
// Purpose: Cloudflare Worker that health-checks Oracle primary and triggers
//          DNS failover to Fly.io when Oracle is unreachable.
// Biblical: Nehemiah 4:9 — "We made our prayer unto God, and set a watch"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-12
//
// Runs on Cloudflare's edge (not on Oracle), so it detects Oracle outages.
// Uses Cloudflare DNS API to flip CNAME records from tunnel to Fly.io.
//
// Environment variables (set via wrangler secret):
//   CF_API_TOKEN — Cloudflare API token with DNS:Edit permission
//                  (from cloudflared cert.pem on Oracle)
//
// Cron trigger: runs every 2 minutes via Cloudflare Workers Cron
// ═══════════════════════════════════════════════════════════════════════════

// ── SETUP ────────────────────────────────────────────────────────────────

const ZONE_ID = "9667dd6162b5b20c8fd6a05e1dcd386d";
const TUNNEL_TARGET = "a441a8db-0905-4e12-ad47-02c68ce607fe.cfargotunnel.com";
const FLYIO_TARGET = "cws-server.fly.dev";

// Subdomains that should failover (dashboard + cpisi route through tunnel)
const FAILOVER_RECORDS = [
  { name: "dashboard.creativeworkzstudio.com", id: null }, // populated at runtime
  { name: "cpisi.creativeworkzstudio.com", id: null },
];

// Health check endpoints (check multiple to avoid false positives)
const HEALTH_CHECKS = [
  "https://dashboard.creativeworkzstudio.com/api/version",
  "https://cpisi.creativeworkzstudio.com/api/version",
];

const CONSECUTIVE_FAILURES_THRESHOLD = 3; // Fail 3 times before triggering

// ── BODY ─────────────────────────────────────────────────────────────────

async function checkHealth(url) {
  try {
    const response = await fetch(url, {
      signal: AbortSignal.timeout(10000), // 10s timeout
      headers: { "User-Agent": "CWS-Failover-Worker/1.0" },
    });
    return response.ok;
  } catch {
    return false;
  }
}

async function getFailureCount(kv) {
  const count = await kv.get("failover:consecutive_failures");
  return parseInt(count || "0", 10);
}

async function setFailureCount(kv, count) {
  await kv.put("failover:consecutive_failures", count.toString());
}

async function getCurrentState(kv) {
  return (await kv.get("failover:state")) || "primary"; // "primary" or "failover"
}

async function setState(kv, state) {
  await kv.put("failover:state", state);
}

async function updateDNS(apiToken, recordName, target) {
  // Find the record ID
  const listResp = await fetch(
    `https://api.cloudflare.com/client/v4/zones/${ZONE_ID}/dns_records?name=${recordName}&type=CNAME`,
    { headers: { Authorization: `Bearer ${apiToken}`, "Content-Type": "application/json" } }
  );
  const listData = await listResp.json();
  if (!listData.success || listData.result.length === 0) {
    console.log(`DNS record not found: ${recordName}`);
    return false;
  }

  const recordId = listData.result[0].id;
  const currentTarget = listData.result[0].content;

  if (currentTarget === target) {
    console.log(`${recordName} already points to ${target}`);
    return true;
  }

  // Update the record
  const updateResp = await fetch(
    `https://api.cloudflare.com/client/v4/zones/${ZONE_ID}/dns_records/${recordId}`,
    {
      method: "PATCH",
      headers: { Authorization: `Bearer ${apiToken}`, "Content-Type": "application/json" },
      body: JSON.stringify({ content: target, proxied: true }),
    }
  );
  const updateData = await updateResp.json();
  console.log(`Updated ${recordName} → ${target}: ${updateData.success}`);
  return updateData.success;
}

async function handleScheduled(event, env) {
  const kv = env.FAILOVER_KV;
  const apiToken = env.CF_API_TOKEN;

  // Check all health endpoints
  const results = await Promise.all(HEALTH_CHECKS.map(checkHealth));
  const allDown = results.every((r) => !r);
  const anyUp = results.some((r) => r);

  const currentState = await getCurrentState(kv);
  let failures = await getFailureCount(kv);

  console.log(`Health check: ${results.map((r, i) => `${HEALTH_CHECKS[i]}: ${r ? "UP" : "DOWN"}`).join(", ")}`);
  console.log(`State: ${currentState}, Consecutive failures: ${failures}`);

  if (allDown) {
    failures++;
    await setFailureCount(kv, failures);

    if (failures >= CONSECUTIVE_FAILURES_THRESHOLD && currentState === "primary") {
      console.log(`FAILOVER: Oracle down ${failures} consecutive checks. Switching to Fly.io.`);
      for (const record of FAILOVER_RECORDS) {
        await updateDNS(apiToken, record.name, FLYIO_TARGET);
      }
      await setState(kv, "failover");
    }
  } else if (anyUp) {
    // Oracle is back
    if (failures > 0) {
      console.log(`Recovery: Oracle back up after ${failures} failures`);
      await setFailureCount(kv, 0);
    }

    if (currentState === "failover") {
      console.log(`RECOVERY: Oracle back. Switching DNS back to tunnel.`);
      for (const record of FAILOVER_RECORDS) {
        await updateDNS(apiToken, record.name, TUNNEL_TARGET);
      }
      await setState(kv, "primary");
    }
  }
}

// ── CLOSING ──────────────────────────────────────────────────────────────

export default {
  async scheduled(event, env, ctx) {
    ctx.waitUntil(handleScheduled(event, env));
  },

  // Manual trigger for testing
  async fetch(request, env) {
    if (new URL(request.url).pathname === "/check") {
      const results = await Promise.all(HEALTH_CHECKS.map(checkHealth));
      const kv = env.FAILOVER_KV;
      const state = await getCurrentState(kv);
      const failures = await getFailureCount(kv);
      return new Response(
        JSON.stringify({
          health: results.map((r, i) => ({ url: HEALTH_CHECKS[i], up: r })),
          state,
          consecutiveFailures: failures,
        }),
        { headers: { "Content-Type": "application/json" } }
      );
    }
    return new Response("CWS Failover Worker", { status: 200 });
  },
};
