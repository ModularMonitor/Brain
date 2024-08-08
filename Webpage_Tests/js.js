const storage = {
    devices: [],
    elements: [],
    time: 0,
    is_mock: false,
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
    }
};

const workers = {
    fcn_update_devices: {
        can_continue: true,
        time: 2000,
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
                                this._update_device_with(i, j);
                            }
                            catch(err) {
                                console.log(`ERR: ${JSON.stringify(err)}`);
                            }
                        }
                    }
        
                    self["step"] = {
                        dev_max: total,
                        dev_now: 0, // loop 0..max
                        curr_dev_max_idx: 0, /* max_index */
                        curr_dev_idx: -1 /* if -1, read max_index and props, then ++ until < max_idx */
                    };

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
                console.log(`ERR: ${JSON.stringify(err)}`);
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
                console.log(`ERR: ${JSON.stringify(err)}`);
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
                console.log(`ERR: ${JSON.stringify(err)}`);
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
                const els = document.getElementById("blob-devices");
                const total = storage.devices.length;
                const exists = els.children.length;

                if (exists != total) {
                    while(els.children.length > total) 
                        els.removeChild(els.children[els.children.length - 1]);

                    while(els.children.length < total) {
                        const nel = document.createElement("device_custom");
                        const id = els.children.length;

                        nel.setAttribute("name", storage.devices[id].name);
                        nel.setAttribute("id", `${id}`);

                        const nel_titl = document.createElement("h2");
                        const nel_desc = document.createElement("p");
                        const nel_ctx = document.createElement("canvas");

                        nel_titl.innerText = "Carregando...";
                        nel_desc.innerText = "Carregando...";

                        nel.appendChild(nel_titl);
                        nel.appendChild(nel_desc);
                        nel.appendChild(nel_ctx);
                        els.appendChild(nel);
                    }

                    storage.elements = [];

                    for(let i = 0; i < els.children.length; ++i) {
                        storage.elements[i] = els.children[i];
                    }
                }

                for(let i = 0; i < storage.elements.length; ++i) {
                    const dev = storage.devices[i];
                    if (!dev) break;

                    const el = storage.elements[i];

                    if (dev["empty"]) {
                        el.children[0].innerText = "Carregando...";
                        el.children[1].innerText = "Por favor, aguarde.";
                        continue;
                    }

                    el.children[0].innerText = dev.name;
                    el.children[1].innerText = `${dev.online ? "online" : "offline"} | ${dev.has_issues ? "com problemas" : "sem problemas"} | ${dev.data.length} propriedades`;

                    if (dev.has_issues) el.classList.add("issues");
                    else {
                        el.classList.remove("issues");
                        if (dev.online) el.classList.add("online");
                        else el.classList.remove("online");
                    }
                }

            }
            catch(err) {
                console.log(`ERR: ${JSON.stringify(err)}`);
            }

            self.can_continue = true;
        }
    }
};

const testers = {
    prepare: function(){
        const json_example = '[{"online":true,"has_issues":false,"last_updated":556651,"name":"DHT22_SENSOR","data":[{"path":"/dht/humidity","values":[44.5,44.599998,44.799999,44.599998,44.799999,44.400002,44.599998,44.700001,44.599998,44.700001,44.700001,44.900002,44.599998,44.900002,44.700001,44.900002,44.5,44.700001,44.599998,44.599998]},{"path":"/dht/temperature","values":[26.9,26.9,27,26.9,27,26.9,26.9,26.9,26.9,26.9,26.9,27,26.799999,26.9,26.799999,26.799999,26.799999,26.799999,26.9,26.9]}]},{"online":false,"has_issues":false,"last_updated":0,"name":"MICS_6814_SENSOR","data":[{}]},{"online":false,"has_issues":false,"last_updated":0,"name":"LY038_HW072_SENSOR","data":[{}]},{"online":true,"has_issues":false,"last_updated":595549,"name":"GY87_SENSOR","data":[{"path":"/gy87/bmp085/altitude","values":[898.165771,898.165771,898.71106,898.165771,898.528931,897.711548,898.256836,898.619995,897.984192,898.347351,897.984192,898.528931,898.71106,897.711548,898.074707,898.347351,898.256836,898.528931,898.074707,898.438416]},{"path":"/gy87/bmp085/altitude_real","values":[912.607483,912.335022,912.69812,912.69812,912.335022,912.244385,912.789307,912.789307,912.425659,911.608765,912.69812,912.425659,912.789307,912.69812,912.335022,912.153748,912.879944,912.789307,912.69812,912.69812]},{"path":"/gy87/bmp085/pressure","values":[90990,90989,90996,90989,90989,90989,90992,90986,90990,90988,90989,90995,90990,90984,90989,90991,90991,90986,90987,90988]},{"path":"/gy87/bmp085/pressure_sea_level","values":[90991,90993,90993,90988,90992,90995,90990,90990,90992,90988,90988,90990,90989,90985,90985,90987,90994,90990,90989,90989]}]},{"online":true,"has_issues":true,"last_updated":513481,"name":"CCS811_SENSOR","data":[{"path":"/ccs811/eco2","values":[409,441,506,439,447,434,423,460,410,422,496,408,433,445,445,475,441,415,441,481]},{"path":"/ccs811/tvoc","values":[1,6,16,5,7,5,3,9,1,3,14,1,5,6,6,11,6,2,6,12]}]},{"online":false,"has_issues":false,"last_updated":0,"name":"PMSDS011_SENSOR","data":[{}]},{"online":false,"has_issues":false,"last_updated":0,"name":"BATTERY_SENSOR","data":[{}]}]';
        storage.devices = JSON.parse(json_example);        
    }
};

/* arg: true -> for local test without server (ESP32) */
function start_timers(only_testing_ones) {
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