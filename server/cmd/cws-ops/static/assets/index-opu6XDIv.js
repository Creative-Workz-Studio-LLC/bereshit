var P=Object.defineProperty;var A=(e,t,a)=>t in e?P(e,t,{enumerable:!0,configurable:!0,writable:!0,value:a}):e[t]=a;var L=(e,t,a)=>A(e,typeof t!="symbol"?t+"":t,a);(function(){const t=document.createElement("link").relList;if(t&&t.supports&&t.supports("modulepreload"))return;for(const s of document.querySelectorAll('link[rel="modulepreload"]'))n(s);new MutationObserver(s=>{for(const r of s)if(r.type==="childList")for(const l of r.addedNodes)l.tagName==="LINK"&&l.rel==="modulepreload"&&n(l)}).observe(document,{childList:!0,subtree:!0});function a(s){const r={};return s.integrity&&(r.integrity=s.integrity),s.referrerPolicy&&(r.referrerPolicy=s.referrerPolicy),s.crossOrigin==="use-credentials"?r.credentials="include":s.crossOrigin==="anonymous"?r.credentials="omit":r.credentials="same-origin",r}function n(s){if(s.ep)return;s.ep=!0;const r=a(s);fetch(s.href,r)}})();let S=[],g=null,h=null;function M(e,t){S=e,g=t,window.addEventListener("hashchange",k),k()}function k(){if(!g)return;const e=window.location.hash||"#/",t=e.slice(1)||"/",a=S.find(s=>s.path===t)??S[0];if(!a)return;h&&(h(),h=null),g.innerHTML="";const n=a.render();g.appendChild(n),"cleanup"in n&&typeof n.cleanup=="function"&&(h=n.cleanup),document.querySelectorAll(".sidebar-nav a").forEach(s=>{const r=s.getAttribute("href")??"";s.classList.toggle("active",r===e||e===""&&r==="#/")})}class N extends EventTarget{constructor(){super(...arguments);L(this,"state",{status:null,loading:!1,error:null,lastRefresh:0})}get(a){return this.state[a]}set(a,n){this.state[a]=n,this.dispatchEvent(new CustomEvent(a,{detail:n})),this.dispatchEvent(new CustomEvent("change",{detail:{key:a,value:n}}))}subscribe(a,n){const s=r=>n(r.detail);return this.addEventListener(a,s),()=>this.removeEventListener(a,s)}getState(){return{...this.state}}}const c=new N,j="/api/ops";async function B(e){const t=await fetch(`${j}${e}`);if(!t.ok)throw new Error(`${t.status}: ${t.statusText}`);return t.json()}async function C(){c.set("loading",!0),c.set("error",null);try{const e=await B("/status");c.set("status",e),c.set("lastRefresh",Date.now())}catch(e){c.set("error",e instanceof Error?e.message:String(e))}finally{c.set("loading",!1)}}function D(e=15e3){C();const t=setInterval(C,e);return()=>clearInterval(t)}const H=Object.freeze(Object.defineProperty({__proto__:null,refreshStatus:C,startAutoRefresh:D},Symbol.toStringTag,{value:"Module"}));function R(){const e=document.createElement("div"),t=[];function a(){const n=c.get("status"),s=c.get("loading"),r=c.get("error");e.innerHTML=`
      <div class="page-header">
        <h2>Overview</h2>
        <p>Infrastructure health at a glance</p>
      </div>

      ${r?`<div class="card mb-4" style="border-color: var(--color-danger)"><p class="text-danger">${r}</p></div>`:""}

      ${s&&!n?'<p class="text-muted">Loading...</p>':""}

      ${n?U(n):""}
    `}return t.push(c.subscribe("status",a)),t.push(c.subscribe("loading",a)),t.push(c.subscribe("error",a)),a(),Object.assign(e,{cleanup:()=>t.forEach(n=>n())}),e}function U(e){return`
    <!-- Quick Stats -->
    <div class="card-grid mb-4">
      ${F(e)}
      ${I(e.resources)}
    </div>

    <!-- Services -->
    <div class="card mt-4">
      <div class="card-header">
        <h3>Services</h3>
        <span class="badge ${e.services.every(t=>t.active)?"badge-success":"badge-warning"}">
          ${e.services.filter(t=>t.active).length}/${e.services.length} running
        </span>
      </div>
      ${_(e.services)}
    </div>

    <p class="text-muted mt-4" style="font-size: var(--font-size-xs)">
      Host: ${e.host} &middot; Last updated: ${new Date(e.timestamp).toLocaleTimeString()}
    </p>
  `}function F(e){const t=e.healthy;return`
    <div class="card">
      <div class="stat-label">Server Health</div>
      <div class="stat-value ${t?"text-success":"text-danger"}">
        ${t?"Healthy":"Unhealthy"}
      </div>
      <a href="${e.serverUrl}" target="_blank" class="text-muted" style="font-size: var(--font-size-xs)">
        ${e.serverUrl}
      </a>
    </div>
  `}function I(e){if(!e)return"";const t=e.memoryTotal>0?(e.memoryUsed/e.memoryTotal*100).toFixed(0):"0",a=e.diskTotal>0?(e.diskUsed/e.diskTotal*100).toFixed(0):"0";return`
    <div class="card">
      <div class="stat-label">CPU Load</div>
      <div class="stat-value">${e.cpuPercent.toFixed(0)}%</div>
      <div class="text-muted" style="font-size: var(--font-size-xs)">
        ${e.loadAvg1.toFixed(2)} / ${e.loadAvg5.toFixed(2)} / ${e.loadAvg15.toFixed(2)}
      </div>
    </div>
    <div class="card">
      <div class="stat-label">Memory</div>
      <div class="stat-value">${t}%</div>
      <div class="text-muted" style="font-size: var(--font-size-xs)">
        ${y(e.memoryUsed)} / ${y(e.memoryTotal)}
      </div>
    </div>
    <div class="card">
      <div class="stat-label">Disk</div>
      <div class="stat-value">${a}%</div>
      <div class="text-muted" style="font-size: var(--font-size-xs)">
        ${y(e.diskUsed)} / ${y(e.diskTotal)}
      </div>
    </div>
  `}function _(e){return`
    <table class="table">
      <thead>
        <tr>
          <th>Service</th>
          <th>Status</th>
          <th>PID</th>
          <th>Memory</th>
          <th>Uptime</th>
        </tr>
      </thead>
      <tbody>
        ${e.map(t=>`
          <tr>
            <td class="mono">${t.name}</td>
            <td>
              <span class="badge ${t.active?"badge-success":"badge-danger"}">
                ${t.active?"running":"stopped"}
              </span>
            </td>
            <td class="mono text-muted">${t.pid||"—"}</td>
            <td class="mono">${t.memory}</td>
            <td class="text-muted">${t.uptime||"—"}</td>
          </tr>
        `).join("")}
      </tbody>
    </table>
  `}function y(e){return e<1024*1024?`${(e/1024).toFixed(0)}KB`:e<1024*1024*1024?`${(e/(1024*1024)).toFixed(0)}MB`:`${(e/(1024*1024*1024)).toFixed(1)}GB`}const T=[{id:"all",label:"Full Deploy",desc:"Build, compile, sync everything, restart"},{id:"binary",label:"Binary Only",desc:"Go binary + restart service"},{id:"dash",label:"Dashboard",desc:"Frontend + config + themes (no restart)"},{id:"site",label:"Website",desc:"Static site to Oracle + Cloudflare Pages"},{id:"builder",label:"Builder CLI",desc:"Builder dist + deps + Makefile"},{id:"fly",label:"Fly.io Standby",desc:"Deploy to Fly.io failover"},{id:"monitoring",label:"Monitoring",desc:"Prometheus + Grafana configs"}];function W(){const e=document.createElement("div");let t=!1;e.innerHTML=`
    <div class="page-header">
      <h2>Deploy</h2>
      <p>Deploy code, config, and assets to production</p>
    </div>

    <div class="card-grid mb-4">
      ${T.map(i=>`
        <div class="card" style="cursor: pointer;" data-target="${i.id}">
          <div class="stat-label">${i.label}</div>
          <div class="text-muted" style="font-size: var(--font-size-xs); margin-top: var(--space-1);">${i.desc}</div>
        </div>
      `).join("")}
    </div>

    <div id="deploy-output-wrap" style="display: none;">
      <div class="card mb-4">
        <div class="card-header">
          <span id="deploy-title">Deploy Output</span>
          <button id="deploy-cancel" class="btn btn-sm btn-danger" style="display: none;">Cancel</button>
        </div>
        <div class="terminal" id="deploy-output" style="height: 40vh; overflow-y: auto;"></div>
      </div>
    </div>
  `;const a=e.querySelector("#deploy-output-wrap"),n=e.querySelector("#deploy-output"),s=e.querySelector("#deploy-title"),r=e.querySelector("#deploy-cancel");T.forEach(i=>{const d=e.querySelector(`[data-target="${i.id}"]`);d&&d.addEventListener("click",()=>l(i))}),r.addEventListener("click",()=>{t=!1,n.textContent+=`
[Cancelled by user]
`,r.style.display="none",o(!0)});function l(i){t||!confirm(`Deploy ${i.label} to production?`)||(t=!0,o(!1),a.style.display="",s.textContent=`Deploying: ${i.label}`,r.style.display="",n.textContent="",fetch(`/api/ops/deploy/${i.id}`,{method:"POST"}).then(async p=>{if(!p.ok){const b=await p.json();n.textContent=`Error: ${b.error}`;return}const u=p.body.getReader(),v=new TextDecoder;let f="";for(;;){const{done:b,value:q}=await u.read();if(b)break;f+=v.decode(q,{stream:!0});const w=f.split(`
`);f=w.pop();for(const $ of w)$.startsWith("data: ")?(n.textContent+=$.slice(6)+`
`,n.scrollTop=n.scrollHeight):$.startsWith("event: done")&&(n.textContent+=`
Deploy complete.
`)}}).catch(p=>{n.textContent+=`
Error: ${p}
`}).finally(()=>{t=!1,r.style.display="none",o(!0)}))}function o(i){e.querySelectorAll("[data-target]").forEach(d=>{d.style.opacity=i?"1":"0.5",d.style.pointerEvents=i?"auto":"none"})}return Object.assign(e,{cleanup:()=>{}}),e}const G=["cws-server","cloudflared","uptime-kuma","litestream","prometheus","node-exporter","grafana-server"];function V(){const e=document.createElement("div");let t=null;e.innerHTML=`
    <div class="page-header">
      <h2>Logs</h2>
      <p>Stream and search server logs in real time</p>
    </div>

    <div class="card mb-4">
      <div style="display: flex; gap: var(--space-3); align-items: center; flex-wrap: wrap;">
        <div>
          <label class="text-muted" style="font-size: var(--font-size-xs); display: block; margin-bottom: var(--space-1);">Service</label>
          <select id="log-service" class="input" style="min-width: 180px;">
            ${G.map(p=>`<option value="${p}">${p}</option>`).join("")}
          </select>
        </div>
        <div>
          <label class="text-muted" style="font-size: var(--font-size-xs); display: block; margin-bottom: var(--space-1);">Lines</label>
          <select id="log-lines" class="input">
            <option value="25">25</option>
            <option value="50" selected>50</option>
            <option value="100">100</option>
            <option value="200">200</option>
          </select>
        </div>
        <div style="align-self: flex-end;">
          <button id="log-fetch" class="btn">Load Logs</button>
          <button id="log-stream" class="btn">Stream Live</button>
          <button id="log-stop" class="btn btn-danger" style="display: none;">Stop</button>
        </div>
        <div style="align-self: flex-end; margin-left: auto;">
          <button id="log-clear" class="btn btn-sm">Clear</button>
        </div>
      </div>
    </div>

    <div class="terminal" id="log-output" style="height: 60vh; overflow-y: auto;"></div>
  `;const a=e.querySelector("#log-service"),n=e.querySelector("#log-lines"),s=e.querySelector("#log-output"),r=e.querySelector("#log-fetch"),l=e.querySelector("#log-stream"),o=e.querySelector("#log-stop"),i=e.querySelector("#log-clear");r.addEventListener("click",async()=>{const p=a.value,u=n.value;r.disabled=!0,r.textContent="Loading...";try{const f=await(await fetch(`/api/ops/logs?service=${p}&lines=${u}`)).json();f.logs&&(s.textContent=f.logs,s.scrollTop=s.scrollHeight)}catch(v){s.textContent=`Error: ${v}`}finally{r.disabled=!1,r.textContent="Load Logs"}}),l.addEventListener("click",()=>{d();const p=a.value,u=n.value;s.textContent=`Streaming ${p} logs...

`,l.style.display="none",o.style.display="",t=new EventSource(`/api/ops/logs/stream?service=${p}&lines=${u}`),t.onmessage=v=>{s.textContent+=v.data+`
`,s.scrollTop=s.scrollHeight},t.onerror=()=>{s.textContent+=`
[Connection closed]
`,d()}}),o.addEventListener("click",()=>d()),i.addEventListener("click",()=>{s.textContent=""});function d(){t&&(t.close(),t=null),l.style.display="",o.style.display="none"}return Object.assign(e,{cleanup:()=>d()}),e}const K="modulepreload",Q=function(e){return"/"+e},z={},J=function(t,a,n){let s=Promise.resolve();if(a&&a.length>0){document.getElementsByTagName("link");const l=document.querySelector("meta[property=csp-nonce]"),o=(l==null?void 0:l.nonce)||(l==null?void 0:l.getAttribute("nonce"));s=Promise.allSettled(a.map(i=>{if(i=Q(i),i in z)return;z[i]=!0;const d=i.endsWith(".css"),p=d?'[rel="stylesheet"]':"";if(document.querySelector(`link[href="${i}"]${p}`))return;const u=document.createElement("link");if(u.rel=d?"stylesheet":K,d||(u.as="script"),u.crossOrigin="",u.href=i,o&&u.setAttribute("nonce",o),document.head.appendChild(u),d)return new Promise((v,f)=>{u.addEventListener("load",v),u.addEventListener("error",()=>f(new Error(`Unable to preload CSS for ${i}`)))})}))}function r(l){const o=new Event("vite:preloadError",{cancelable:!0});if(o.payload=l,window.dispatchEvent(o),!o.defaultPrevented)throw l}return s.then(l=>{for(const o of l||[])o.status==="rejected"&&r(o.reason);return t().catch(r)})};let m=null;function X(){return m||(m=document.createElement("div"),m.className="toast-container",document.body.appendChild(m)),m}function x(e,t="info",a=3e3){const n=document.createElement("div");n.className=`toast toast-${t}`,n.textContent=e,X().appendChild(n),requestAnimationFrame(()=>n.classList.add("toast-visible")),setTimeout(()=>{n.classList.remove("toast-visible"),n.addEventListener("transitionend",()=>n.remove())},a)}function Y(){const e=document.createElement("div"),t=[];function a(){const n=c.get("status"),s=c.get("loading"),r=(n==null?void 0:n.services)??[];e.innerHTML=`
      <div class="page-header">
        <h2>Services</h2>
        <p>Manage systemd services — start, stop, restart</p>
      </div>

      ${s&&!n?'<p class="text-muted">Loading...</p>':""}

      <div class="card-grid">
        ${r.map(l=>Z(l)).join("")}
      </div>
    `,r.forEach(l=>{const o=e.querySelector(`[data-service="${l.name}"]`);o&&o.querySelectorAll("[data-action]").forEach(i=>{i.addEventListener("click",()=>{const d=i.dataset.action;ee(l.name,d)})})})}return t.push(c.subscribe("status",a)),t.push(c.subscribe("loading",a)),a(),Object.assign(e,{cleanup:()=>t.forEach(n=>n())}),e}function Z(e){const t=e.active?"badge-success":"badge-danger",a=e.active?"running":"stopped";return`
    <div class="card" data-service="${e.name}">
      <div class="card-header">
        <span class="mono">${e.name}</span>
        <span class="badge ${t}">${a}</span>
      </div>
      <div style="display: flex; gap: var(--space-2); flex-wrap: wrap; margin-top: var(--space-3);">
        <div style="flex: 1; min-width: 120px;">
          <div class="text-muted" style="font-size: var(--font-size-xs)">PID</div>
          <div class="mono">${e.pid||"—"}</div>
        </div>
        <div style="flex: 1; min-width: 120px;">
          <div class="text-muted" style="font-size: var(--font-size-xs)">Memory</div>
          <div class="mono">${e.memory}</div>
        </div>
        <div style="flex: 1; min-width: 120px;">
          <div class="text-muted" style="font-size: var(--font-size-xs)">Uptime</div>
          <div class="mono">${e.uptime||"—"}</div>
        </div>
      </div>
      <div style="display: flex; gap: var(--space-2); margin-top: var(--space-3);">
        <button class="btn btn-sm" data-action="restart">Restart</button>
        ${e.active?'<button class="btn btn-sm btn-danger" data-action="stop">Stop</button>':'<button class="btn btn-sm btn-success" data-action="start">Start</button>'}
      </div>
    </div>
  `}async function ee(e,t){if(!confirm(`${t.charAt(0).toUpperCase()+t.slice(1)} ${e}?`))return;const n=document.querySelector(`[data-service="${e}"] [data-action="${t}"]`);n&&(n.disabled=!0,n.textContent=`${t}ing...`);try{const s=await fetch(`/api/ops/services/${e}/${t}`,{method:"POST"}),r=await s.json();if(!s.ok){x(`Failed to ${t} ${e}: ${r.error}`,"error");return}x(`${e}: ${t} successful`,"success");const{refreshStatus:l}=await J(async()=>{const{refreshStatus:o}=await Promise.resolve().then(()=>H);return{refreshStatus:o}},void 0);await l()}catch(s){x(`Error: ${s}`,"error")}finally{n&&(n.disabled=!1,n.textContent=t.charAt(0).toUpperCase()+t.slice(1))}}const te=[{type:"CNAME",name:"creativeworkzstudio.com",content:"Cloudflare Pages",proxied:!0},{type:"CNAME",name:"www",content:"Cloudflare Pages",proxied:!0},{type:"CNAME",name:"dashboard",content:"Cloudflare Tunnel",proxied:!0},{type:"CNAME",name:"cpisi",content:"Cloudflare Tunnel",proxied:!0},{type:"CNAME",name:"status",content:"Cloudflare Tunnel",proxied:!0},{type:"CNAME",name:"grafana",content:"Cloudflare Tunnel",proxied:!0}];function ne(){const e=document.createElement("div");return e.innerHTML=`
    <div class="page-header">
      <h2>DNS</h2>
      <p>Cloudflare DNS records for creativeworkzstudio.com</p>
    </div>

    <div class="card mb-4">
      <div class="card-header">
        <span>DNS Records</span>
        <a href="https://dash.cloudflare.com" target="_blank" class="btn btn-sm">
          Open Cloudflare Dashboard
        </a>
      </div>
      <table class="table">
        <thead>
          <tr>
            <th>Type</th>
            <th>Name</th>
            <th>Content</th>
            <th>Proxy</th>
          </tr>
        </thead>
        <tbody>
          ${te.map(t=>`
            <tr>
              <td><span class="badge">${t.type}</span></td>
              <td class="mono">${t.name}</td>
              <td class="text-muted">${t.content}</td>
              <td>
                <span class="badge ${t.proxied?"badge-success":"badge-warning"}">
                  ${t.proxied?"proxied":"DNS only"}
                </span>
              </td>
            </tr>
          `).join("")}
        </tbody>
      </table>
    </div>

    <div class="card">
      <div class="card-header">
        <span>Architecture</span>
      </div>
      <div style="padding: var(--space-3); font-size: var(--font-size-sm);">
        <pre class="mono text-muted" style="line-height: 1.8;">User → Cloudflare CDN → Tunnel → Oracle VM (:3847)
                              ├── dashboard.creativeworkzstudio.com  →  Go server
                              ├── cpisi.creativeworkzstudio.com      →  Go server
                              ├── status.creativeworkzstudio.com     →  Uptime Kuma (:3001)
                              └── grafana.creativeworkzstudio.com    →  Grafana (:3000)

User → Cloudflare Pages  →  creativeworkzstudio.com (static site)
User → Fly.io (standby)  →  cws-server.fly.dev (auto-stopped)</pre>
      </div>
    </div>
  `,e}function se(){const e=document.createElement("div");e.innerHTML=`
    <div class="page-header">
      <h2>Backups</h2>
      <p>Litestream replication status and database restore</p>
    </div>

    <div id="backup-content">
      <p class="text-muted">Loading backup status...</p>
    </div>

    <div id="verify-output-wrap" style="display: none;" class="mt-4">
      <div class="card">
        <div class="card-header">
          <span>Integrity Check</span>
        </div>
        <div class="terminal" id="verify-output" style="max-height: 30vh; overflow-y: auto;"></div>
      </div>
    </div>
  `;const t=e.querySelector("#backup-content"),a=e.querySelector("#verify-output-wrap"),n=e.querySelector("#verify-output");async function s(){try{const i=await(await fetch("/api/ops/backups")).json();r(i)}catch(o){t.innerHTML=`<div class="card"><p class="text-danger">Error loading backup status: ${o}</p></div>`}}function r(o){const i=o.litestreamRunning?"running":"stopped",d=o.litestreamRunning?"badge-success":"badge-danger";t.innerHTML=`
      <div class="card mb-4">
        <div class="card-header">
          <span>Litestream</span>
          <span class="badge ${d}">${i}</span>
        </div>
        <p class="text-muted" style="font-size: var(--font-size-xs); margin-top: var(--space-2);">
          Continuous SQLite replication to Cloudflare R2
        </p>
      </div>

      ${o.databases.length>0?`
        <div class="card">
          <div class="card-header">
            <span>Databases</span>
            <button id="verify-btn" class="btn btn-sm">Verify Integrity</button>
          </div>
          <table class="table">
            <thead>
              <tr>
                <th>Database</th>
                <th>Size</th>
                <th>Replicating</th>
              </tr>
            </thead>
            <tbody>
              ${o.databases.map(u=>`
                <tr>
                  <td class="mono">${u.name}</td>
                  <td class="mono">${u.sizeHuman}</td>
                  <td>
                    <span class="badge ${u.replicating?"badge-success":"badge-danger"}">
                      ${u.replicating?"yes":"no"}
                    </span>
                  </td>
                </tr>
              `).join("")}
            </tbody>
          </table>
        </div>
      `:'<p class="text-muted">No databases found</p>'}
    `;const p=t.querySelector("#verify-btn");p&&p.addEventListener("click",l)}async function l(){const o=t.querySelector("#verify-btn");o&&(o.disabled=!0,o.textContent="Verifying..."),a.style.display="",n.textContent=`Running integrity check...
`;try{const i=await fetch("/api/ops/backups/verify",{method:"POST"}),d=await i.json();i.ok?n.textContent=d.output||"No output":n.textContent=`Error: ${d.error}`}catch(i){n.textContent=`Error: ${i}`}finally{o&&(o.disabled=!1,o.textContent="Verify Integrity")}}return s(),Object.assign(e,{cleanup:()=>{}}),e}const E=[{path:"/",label:"Overview",render:R},{path:"/deploy",label:"Deploy",render:W},{path:"/logs",label:"Logs",render:V},{path:"/services",label:"Services",render:Y},{path:"/dns",label:"DNS",render:ne},{path:"/backups",label:"Backups",render:se}];function ae(){const e=document.getElementById("app");return e.innerHTML=`
    <aside class="sidebar">
      <div class="sidebar-header">
        <h1>CWS Ops</h1>
        <div class="subtitle">Infrastructure Management</div>
      </div>
      <nav class="sidebar-nav">
        ${E.map(t=>`<a href="#${t.path}">${t.label}</a>`).join("")}
      </nav>
      <div style="padding: var(--space-4); border-top: 1px solid var(--color-border);">
        <div class="connection-status" id="connection-status">
          <span class="connection-dot"></span>
          <span>Connected</span>
        </div>
        <div style="margin-top: var(--space-2); font-size: var(--font-size-xs); color: var(--color-text-dim);">
          v0.1.0 &middot; CreativeWorkzStudio
        </div>
      </div>
    </aside>
    <main class="main-content" id="page-content"></main>
  `,document.getElementById("page-content")}const re=ae();M(E,re);D(15e3);function O(){const e=document.querySelector("#connection-status .connection-dot"),t=document.querySelector("#connection-status span:last-child");if(!e||!t)return;const a=c.get("error"),n=c.get("loading");a?(e.className="connection-dot disconnected",t.textContent="Disconnected"):n?(e.className="connection-dot connecting",t.textContent="Refreshing..."):(e.className="connection-dot",t.textContent="Connected")}c.subscribe("error",O);c.subscribe("loading",O);document.addEventListener("keydown",e=>{if(e.target instanceof HTMLInputElement||e.target instanceof HTMLTextAreaElement||e.target instanceof HTMLSelectElement)return;const t=parseInt(e.key)-1,a=E[t];a&&(window.location.hash=`#${a.path}`)});
