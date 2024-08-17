#pragma once

#include <Arduino.h>

#include "defaults.h"
#include "SD_card.h"


#ifdef USE_SD_CARD_AS_REF
inline String& _get_webserver_idx(const int off) {
    static String pages[3];
    return pages[off];
}

inline void reload_webserver_items() {
    auto& sd = GET(MySDcard);

    for(int i = 0; i < 3; ++i) {
        String& buf = _get_webserver_idx(i);
        buf.clear();

        char tmp_buf[web_slice_read];
        size_t off = 0, current_read = 0;

        const char* page = [&i]() -> const char* { switch(i){
            case 0: return web_file_html;
            case 1: return web_file_js;
            default: return web_file_css;
        }}();

        const size_t file_expected_size = sd.get_file_size(page);

        LOGI(e_LOG_TAG::TAG_WIFI, "Reloading webpage index %i '%s' of size %zu...", i, page, file_expected_size);

        while(off < file_expected_size) {
            current_read = sd.read_from(page, tmp_buf, web_slice_read, off);
            off += current_read;
            if (current_read) {
                buf += String(tmp_buf, current_read);
            }
            else {
                LOGW_NOSD(e_LOG_TAG::TAG_WIFI, "Got zero bytes read. Current: %zu, progress: %.2f%%. Waiting a second before retry.", off, (100.0f * off / file_expected_size));
                SLEEP(1000);
            }
        }

        LOGI(e_LOG_TAG::TAG_WIFI, "Reloaded webpage index %i '%s' now with size %u.", i, page, buf.length());
    }
}
inline const String& get_webserver_home()
{
    String& ref = _get_webserver_idx(0);
    if (ref.isEmpty()) reload_webserver_items();
    return ref;
}

inline const String& get_webserver_js()
{
    String& ref = _get_webserver_idx(1);
    if (ref.isEmpty()) reload_webserver_items();
    return ref;
}

inline const String& get_webserver_css()
{
    String& ref = _get_webserver_idx(2);
    if (ref.isEmpty()) reload_webserver_items();
    return ref;
}
#else
inline const String& get_webserver_home() {
    static const String __data = R"===(<!DOCTYPE html>
<html lang="pt-br">
<head>
    <META NAME="KEYWORDS" CONTENT="html, css, javascript">
    <meta name="rating" content="general">
    <meta name="generator" content="VSCode">
    <meta http-equiv="pragma" content="no-cache">
    <meta name="copyright" content="none">
    <meta charset="utf-8">
    <meta http-equiv="content-language" content="pt-br">
    <meta name="author" content="ModularMonitor">
    <meta name="description" content="Device page">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    
    <title>Modular Monitor Page</title>

    <script type="text/javascript" src="/js.js" defer></script>
    <link rel="stylesheet" href="/css.css">
</head>
<body>
    <section class="header">
        <h1>ModularMonitor Device</h1>
        <hr>
        <h4>Tempo ligado: <span id="el-date">...</span> | Dispositivos online: <span id="el-online">...</span> de <span id="el-total">...</span></h4>
    </section>
    <hr>
    <section class="content" id="blob-devices">
    </section>
</body>
</html>)===";
    return __data;
}
inline const String& get_webserver_js() {
    static const String __data = R"===(const storage = {
    devices: [],
    elements: [],
    time: 0,
    is_mock: false,
    last_err: null,
    exclusivity_click_ev: 0, /* 0 == devices or section, 1 == select. Resets on root event */
    page_start_time: Number(new Date())
};

const cte = {
    max_tries_exception_http_req: 3
};

const tools = {
    req_async: async function(path) {
        const method = "GET";
        const url = `http://modularmonitor.local${path}`;

        return JSON.parse(await new Promise(function (resolve, reject) {
            let xhr = new XMLHttpRequest();
            xhr.open(method, url);
            xhr.onload = function () {
                if (this.status >= 200 && this.status < 300) {
                    resolve(xhr.response);
                } else {
                    reject({
                        status: this.status,
                        statusText: xhr.statusText
                    });
                }
            };
            xhr.onerror = function () {
                reject({
                    status: this.status,
                    status: this.status,
                    status: this.status,
                    statusText: xhr.statusText
                });
            };
            xhr.send();
        }));
    },

    sec2str: function(seconds)
    {
        const sec = Math.floor(seconds) % 60;
        const min = Math.floor(seconds / 60) % 60;
        const hour = Math.floor(seconds / 3600);

        return hour > 0 ? `${hour}h${min}m${sec}s` :
            (min  > 0 ? `${min}m${sec}s` : `${sec}s`);
    },

    log: function(prefix, text) {
        console.log(`#${Number(new Date())} ${(prefix ? prefix : "INFO")}: ${text}`);
    },

    fill_device_list: function(total) {
        const els = document.getElementById("blob-devices");

        document.body.addEventListener("click", function() {
            setTimeout(function() {
                storage.exclusivity_click_ev = 0;
            }, 10);
        });

        while(els.children.length > 0) /* reset */
            els.removeChild(els.children[els.children.length - 1]);

        while(els.children.length < total) {
            const nel = document.createElement("device_custom");
            const id = els.children.length;

            nel.setAttribute("name", storage.devices[id].name);
            nel.setAttribute("id", `${id}`);
            nel.setAttribute("offset", `${id}`);
            nel.setAttribute("selected", "false");
            nel.addEventListener("click", function() {
                if (storage.exclusivity_click_ev != 0) 
                    return;

                if (nel.getAttribute("selected") == "false") {
                    let _nel = document.getElementById("el-show-big");
                    if (_nel != null) {
                        tools.log("WARN", `At ${id}, el-show-big exists already! Clicked out of screen. Workaround applied.`);
                        _nel.children[0].click();
                        return;
                    }
                    else {
                        _nel = document.createElement("section");
                    }
                    nel.setAttribute("selected", "true");
                    _nel.className = "popup";
                    _nel.setAttribute("id", "el-show-big");
                    _nel.appendChild(nel);
                    _nel.addEventListener("click", function(){
                        nel.click();
                    });
                    document.body.appendChild(_nel);
                }
                else {
                    nel.setAttribute("selected", "false");
                    els.insertBefore(nel, els.children[Number(nel.getAttribute("offset"))]);
                    const _nel = document.getElementById("el-show-big");
                    if (_nel) _nel.parentNode.removeChild(_nel);
                }
            });

            const nel_titl = document.createElement("h2");
            const nel_desc = document.createElement("p");
            const nel_sel = document.createElement("select");
            const nel_ctx = document.createElement("canvas");

            nel_titl.innerText = "Carregando...";

            nel_desc.innerText = "Carregando...";

            nel_sel.setAttribute("name", "property selector");
            nel_sel.setAttribute("id", "select");
            nel_sel.addEventListener("click", function(ev) {
                storage.exclusivity_click_ev = 1;
            });

            nel.appendChild(nel_titl);
            nel.appendChild(nel_desc);
            nel.appendChild(nel_sel);
            nel.appendChild(nel_ctx);

            els.appendChild(nel);
        }

        storage.elements = [];
        for(let i = 0; i < els.children.length; ++i) {
            storage.elements[i] = els.children[i];
        }
    }
};

const workers = {
    fcn_update_devices: {
        can_continue: true,
        time: 500,
        step: null,
        is_debug_ready: false,
        pretty_name: "Update devices one by one forever",

        _update_device_with: function(dev_now, json) {
            storage.devices[dev_now]["online"] = json["online"];
            storage.devices[dev_now]["has_issues"] = json["has_issues"];
            storage.devices[dev_now]["last_updated"] = json["last_updated"];
            storage.devices[dev_now]["name"] = json["name"];

            if (!storage.devices[dev_now]["data"]) storage.devices[dev_now]["data"] = [];

            storage.devices[dev_now]["empty"] = false;
        },

        fcn: async function() {
            const self = workers.fcn_update_devices;
            if (!self.can_continue) return;            
        
            self.can_continue = false;
            try {
                if (self.step == null) { /* configure */
                    const j = await tools.req_async("/get_device/count");
                    const total = j["count"];
                    /* init if needed */
                    for(let i = 0; i < total; ++i) {
                        if (storage.devices[i] == null) {
                            storage.devices[i] = { empty: true };
                            try {
                                const j = await tools.req_async(`/get_device/${i}?resumed=true`);
                                self._update_device_with(i, j);
                            }
                            catch(err) {
                                console.log(`ERR: ${(storage.last_err = err)}`);
                            }
                        }
                    }
        
                    self["step"] = {
                        dev_max: total,
                        dev_now: 0, /* loop 0..max */
                        curr_dev_max_idx: 0, /* max_index */
                        curr_dev_idx: -1 /* if -1, read max_index and props, then ++ until < max_idx */
                    };
                    
                    tools.fill_device_list(total);
                }
                else {
                    if (self.step.curr_dev_idx == -1) {
                        const j = await tools.req_async(`/get_device/${self.step.dev_now}`);

                        self._update_device_with(self.step.dev_now, j);
                        
                        self.step.curr_dev_max_idx = Number(j["max_index"]);
                    }
                    else {
                        const j = await tools.req_async(`/get_device/${self.step.dev_now}?index=${self.step.curr_dev_idx}`);
                        
                        storage.devices[self.step.dev_now]["data"][self.step.curr_dev_idx] = {
                            path: j["path"],
                            values: j["data"]
                        };
                    }
                        
                    if (++self.step.curr_dev_idx >= self.step.curr_dev_max_idx) {
                        self.step.curr_dev_idx = -1;
                        self.step.dev_now = (self.step.dev_now + 1) % self.step.dev_max;
                    }
                }
            }
            catch(err) {
                console.log(`ERR: ${(storage.last_err = err)}`);
            }
            self.can_continue = true;
        }
    },
    fcn_update_time: {
        can_continue: true,
        time: 1000,
        is_debug_ready: true,
        pretty_name: "Keeps timer up to date with device's uptime",

        fcn: async function() {
            const self = workers.fcn_update_time;
            if (!self.can_continue) return;
            self.can_continue = false;
        
            try {
                const el = document.getElementById("el-date");
                if (storage.is_mock) {
                    device_time = (Number(new Date()) - storage.page_start_time) / 1000;
                }
                else {
                    device_time = Number((await tools.req_async("/time"))["time"]) / 1000;
                }
                el.innerText = tools.sec2str(device_time);
            }
            catch(err) {
                console.log(`ERR: ${(storage.last_err = err)}`);
            }
            self.can_continue = true;
        }
    },
    fcn_update_devices_online_and_total: {
        can_continue: true,
        time: 10000,
        is_debug_ready: true,
        pretty_name: "Keeps online and total devices count up to date",

        fcn: function() {
            const self = workers.fcn_update_devices_online_and_total;
            if (!self.can_continue) return;
            self.can_continue = false;
        
            try {
                const el_on = document.getElementById("el-online");
                const el_tot = document.getElementById("el-total");
                let counter = 0;
                storage.devices.forEach(function(e) { if (e["online"]) ++counter; }); 
                el_on.innerText = `${counter}`;
                el_tot.innerText = `${storage.devices.length}`;
            }
            catch(err) {
                console.log(`ERR: ${(storage.last_err = err)}`);
            }
            self.can_continue = true;
        }
    },
    fcn_update_devices_list: {
        can_continue: true,
        time: 1000,
        is_debug_ready: true,
        pretty_name: "Keeps list up to date with update_time task data",

        fcn: function() {
            const self = workers.fcn_update_devices_list;
            if (!self.can_continue) return;
            self.can_continue = false;

            try {
                for(let i = 0; i < storage.elements.length; ++i) {
                    const dev = storage.devices[i];
                    if (!dev) continue;

                    const el = storage.elements[i];

                    const title = el.getElementsByTagName("h2")[0];
                    const paragraph = el.getElementsByTagName("p")[0];
                    const select = el.getElementsByTagName("select")[0];
                    const canvas = el.getElementsByTagName("canvas")[0];

                    if (dev["empty"]) {
                        title.innerText = "Carregando...";
                        paragraph.innerText = "Por favor, aguarde.";
                        while(select.children.length > 0) select.removeChild(select.children[0]);
                        continue;
                    }

                    title.innerText = dev.name;
                    paragraph.innerText = `${dev.online ? "online" : "offline"} | ${dev.has_issues ? "com problemas" : "sem problemas"} | ${dev.data.length} propriedades`;

                    for(let k = 0; k < dev.data.length; ++k) {
                        if (select.children.length <= k) select.appendChild(document.createElement("option"));
                        const opt = select.children[k];
                        if (opt.getAttribute("value") == dev.data[k].path) continue;

                        opt.setAttribute("value", "" + dev.data[k].path);
                        opt.innerText = dev.data[k].path;
                    }

                    if (dev.has_issues) el.classList.add("issues");
                    else {
                        el.classList.remove("issues");
                        if (dev.online) el.classList.add("online");
                        else el.classList.remove("online");
                    }

                    /* canvas part */

                    canvas.width = canvas.offsetWidth;
                    canvas.height = canvas.offsetHeight;

                    const ctx = canvas.getContext("2d");
                    ctx.font = "14px 'Segoe UI Emoji', 'Segoe UI Variable', 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif";
                    ctx.clearRect(0, 0, canvas.width, canvas.height);

                    let sel_opt = null;
                    for(let j = 0; j < dev.data.length; ++j) {
                        if (select.value == dev.data[j].path) {
                            sel_opt = dev.data[j];
                            break;
                        }
                    }

                    function draw_text(text, x, y) {
                        ctx.fillStyle = 'black';
                        ctx.fillText(text, x + 1, y + 1);
                        ctx.fillStyle = 'white';
                        ctx.fillText(text, x, y);
                    }

                    if (sel_opt) {
                        ctx.fillStyle = 'black';

                        const total = sel_opt.values.length;
                        const step_x = canvas.width * 1.0 / (total - 1);
                        
                        const max_y = Math.max(...sel_opt.values);
                        const min_y = Math.min(...sel_opt.values);
                        let diff_y = max_y - min_y;
                        if (diff_y == 0.0) diff_y = 0.0000001;
                        
                        ctx.beginPath();                        
                        for(let x = 0; x < total - 1; ++x) {
                            const rx = total - x - 2; /* real offset in canvas */

                            const fx = rx * step_x;
                            const lx = (rx + 1) * step_x;
                            const ly = (1.0 - ((sel_opt.values[x] - min_y) / diff_y)) * canvas.height;
                            const fy = (1.0 - ((sel_opt.values[x + 1] - min_y) / diff_y)) * canvas.height;

                            ctx.moveTo(fx, fy);
                            ctx.lineTo(lx, ly);
                        }
                        ctx.stroke();

                        draw_text(`Max: ${max_y}`, 1.5, 14);
                        draw_text(`Min: ${min_y}`, 1.5, 28);
                    }
                    else {
                        draw_text(`Sem dados`, 1.5, 14);
                    }
                }
            }
            catch(err) {
                console.log(`ERR: ${(storage.last_err = err)}`);
            }

            self.can_continue = true;
        }
    }
};

const testers = {
    prepare: function(){
        const json_example = '[{"empty":false,"online":true,"has_issues":false,"last_updated":737144,"name":"DHT22_SENSOR","data":[{"path":"/dht/humidity","values":[51.099998,50.799999,50.799999,50.599998,50.5,50.299999,50.099998,49.599998,49.5,49.400002,49,49,48.799999,48.700001,48.599998,48.299999,48.200001,48.299999,48.299999,48.400002]},{"path":"/dht/temperature","values":[20.200001,20.200001,20.200001,20.200001,20.200001,20.200001,20.1,20.1,20.1,20.1,20.1,20.1,20.1,20.1,20.1,20,20.1,20,20,20]}]},{"empty":false,"online":false,"has_issues":false,"last_updated":0,"name":"MICS_6814_SENSOR","data":[]},{"empty":false,"online":false,"has_issues":false,"last_updated":0,"name":"LY038_HW072_SENSOR","data":[]},{"empty":false,"online":true,"has_issues":false,"last_updated":693673,"name":"GY87_SENSOR","data":[{"path":"/gy87/bmp085/altitude","values":[897.075806,897.166809,896.985291,896.531128,897.438904,897.711548,896.621643,897.166809,897.075806,897.529968,897.166809,897.711548,897.438904,897.438904,896.985291,896.894226,897.257324,897.348389,898.074707,897.166809]},{"path":"/gy87/bmp085/altitude_real","values":[911.608765,911.245728,911.154541,910.88208,911.608765,911.608765,911.245728,912.153748,911.154541,912.153748,911.790039,911.881226,911.699402,911.154541,911.699402,910.973267,911.699402,911.790039,911.336365,911.881226]},{"path":"/gy87/bmp085/pressure","values":[91004,91000,91006,91005,90997,91006,91003,91007,91000,91000,90999,91004,91003,91002,90998,91002,91005,91005,91002,91003]},{"path":"/gy87/bmp085/pressure_sea_level","values":[91009,91004,91005,91007,91004,91001,91001,91010,90994,91002,91005,90999,91005,91002,91006,91003,91005,91002,91004,91001]},{"path":"/gy87/bmp085/temperature","values":[19.299999,19.200001,19.200001,19.200001,19.1,19.1,19.1,19.1,19.1,19.1,19.1,19.1,19.1,19.1,19.1,19.1,19.1,19.1,19.1,19.1]},{"path":"/gy87/mpu6050/accel/x","values":[0,1,-27,1,2,1,0,6,3,3,3,-1,-4,8,0,7,3,8,5,3]},{"path":"/gy87/mpu6050/accel/x/raw","values":[262,260,-384,210,250,214,224,242,234,234,274,244,220,286,242,248,270,300,238,248]},{"path":"/gy87/mpu6050/accel/y","values":[-128,-134,-130,-125,-123,-131,-130,-124,-125,-135,-134,-123,-137,-125,-126,-126,-131,-127,-125,-133]},{"path":"/gy87/mpu6050/accel/y/raw","values":[-16632,-16636,-16610,-16642,-16620,-16636,-16610,-16568,-16626,-16672,-16660,-16606,-16640,-16608,-16606,-16668,-16590,-16582,-16636,-16658]},{"path":"/gy87/mpu6050/accel/z","values":[19,3,-4,54,11,0,7,-1,4,0,11,3,3,5,9,8,4,19,-4,4]},{"path":"/gy87/mpu6050/accel/z/raw","values":[886,852,772,920,844,758,756,794,810,758,850,762,804,812,836,858,854,782,756,846]},{"path":"/gy87/mpu6050/gyro/x/raw","values":[-101,-102,-102,-100,-101,-101,-102,-101,-101,-101,-101,-101,-102,-101,-101,-100,-101,-101,-101,-101]},{"path":"/gy87/mpu6050/gyro/y/raw","values":[17,18,17,17,17,16,18,18,18,17,18,18,17,18,18,18,17,18,16,17]},{"path":"/gy87/mpu6050/gyro/z/raw","values":[-5,-4,-4,-3,-5,-4,-3,-4,-4,-4,-4,-3,-4,-4,-4,-4,-4,-4,-5,-3]},{"path":"/gy87/mpu6050/pitch","values":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]},{"path":"/gy87/mpu6050/roll","values":[-87,-87,-87,-87,-87,-87,-87,-87,-87,-87,-87,-87,-87,-87,-87,-87,-87,-87,-87,-87]},{"path":"/gy87/mpu6050/yaw","values":[1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]}]},{"empty":false,"online":true,"has_issues":false,"last_updated":731853,"name":"CCS811_SENSOR","data":[{"path":"/ccs811/eco2","values":[420,447,486,530,505,533,518,545,548,503,459,477,443,400,414,520,464,493,482,492]},{"path":"/ccs811/tvoc","values":[3,7,13,19,15,20,17,22,22,3,8,11,6,0,2,18,9,14,12,14]}]},{"empty":false,"online":false,"has_issues":false,"last_updated":0,"name":"PMSDS011_SENSOR","data":[]},{"empty":false,"online":false,"has_issues":false,"last_updated":0,"name":"BATTERY_SENSOR","data":[]}]';
        storage.devices = JSON.parse(json_example);
        tools.fill_device_list(storage.devices.length);
    }
};

/* arg: true -> for local test without server (ESP32) */
function start_stuff(only_testing_ones) {
    const check_is_debug_only = (only_testing_ones === true);    
    const task_list = Object.keys(workers);

    if (check_is_debug_only) {
        storage.is_mock = true;
        tools.log("INFO", `Calling testers.prepare() to test debug.`);
        testers.prepare();
    }
    else {
        storage.is_mock = false;
    }

    for(let i = 0; i < task_list.length; ++i) {
        const each = workers[task_list[i]];
        if (check_is_debug_only && !each.is_debug_ready) {
            tools.log("INFO", `Did not start task '${each.pretty_name}' whose interval is ${each.time / 1000.0} sec because of debug call.`);
        }
        else {
            tools.log("INFO", `Starting task '${each.pretty_name}' whose interval is ${each.time / 1000.0} sec!`);
            setInterval(each.fcn, each.time);
            setTimeout(each.fcn, 0.1 * each.time);
        }
    }
}

setTimeout(function(){ start_stuff(); }, 250);)===";
    return __data;
}
inline const String& get_webserver_css() {
    static const String __data = R"===(html {
    height: 100%;
}
body {
    width: 100%;
    height: 100%;
    box-sizing: border-box;
    background-color: hsl(0, 0%, 50%);
    margin: 0;
    padding: 0;
    font-family: 'Trebuchet MS', 'Lucida Sans Unicode', 'Lucida Grande', 'Lucida Sans', Arial, sans-serif;
}
hr {
    margin: 0;
    height: 2px;
    background-color: black;
    border: black solid 1px;
}
h1 {
    font-family: cursive;
    font-size: x-large;
    font-weight: 700;
    margin: 0.6em 0.1em 0.3em;
}
h4 {
    margin: 1em 0.2em 0.6em;
}

.header {
    margin: 0;
    padding: 0.5em 2em;
    background-color: hsl(0, 0%, 80%);
    box-shadow: inset -3px -3px 2px 3px hsl(0, 0%, 50%), inset 3px 3px 2px 3px hsl(0, 0%, 85%);
}
.header hr {
    background-color: rgb(0, 0, 0);
    box-shadow: 1px 2px 5px 1px hsla(0, 0%, 50%, 0.6);
}

.content {
    margin: 0;
    padding: 1.2em;
    display: flex;
    flex-wrap: wrap;
    gap: 20px;
    box-sizing: border-box;
}

.popup {
    position: absolute;
    width: 100%;
    height: 100%;
    left: 0;
    top: 0;
    align-items: center;
    text-align: center;
    background-color: hsla(0, 0%, 20%, 0.7);
    box-shadow: 0 0 0 2000px hsla(0, 0%, 20%, 0.7); /* I don't have time to make this what I want without this hack */

}

.popup device_custom {
    position: relative;
    display: flex;
    left: 50%;
    top: 50%;
    min-width: 50vw;
    width: 600px;
    transform: translate(-50%, -50%);
}
.popup device_custom canvas {
    width: 100%;
    height: 300px;
}

device_custom {
    width: 250px;
    max-width: 340px;
    height: auto;
    min-height: 100px;
    box-shadow: 3px 5px 6px 3px hsla(0, 0%, 20%, 0.4);
    background-color: hsl(0, 0%, 80%);
    display: flex;
    flex-wrap: wrap;
    flex: 1 1 auto;
    border: 2px solid black;
    border-radius: 10px;
    min-width: min-content;
    pointer-events: all;
}

device_custom.online {
    background-color: hsl(110, 30%, 70%);
}
device_custom.issues {
    background-color: hsl(0, 40%, 60%);
}

device_custom * {
    flex: 1 1 auto;
    width: 100%;
}

device_custom h2 {
    margin: 0.2em 15px 0.6em;
}

device_custom p {
    margin: 0.1em 15px 0.1em;
}

device_custom select {
    margin: 5px 15px 0;
    box-sizing: content-box;
}

device_custom canvas {
    margin: 15px;
    padding: 0;
    width: 100%;
    height: 120px;
    box-sizing: content-box;
    background-color: hsla(0, 0%, 20%, 0.4);
    border: 1px solid black;
})===";
    return __data;
}
// does nothing, it is here just to avoid breaking code that expects SD card issues.
inline void reload_webserver_items() {
    LOGW(e_LOG_TAG::TAG_WIFI, "Reloading is not enabled in this build. Data is static.");
}
#endif
