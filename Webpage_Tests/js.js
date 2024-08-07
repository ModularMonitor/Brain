const storage = {
    devices: [],
    time: 0,
};

const cte = {
    max_tries_exception_http_req: 3
};

const workers = {
    fcn_update_devices: {
        fcn: null,
        can_continue: true,
        time: 5000,
        step: null
    },
    fcn_update_time: {
        fcn: null,
        can_continue: true,
        time: 1000
    },
    fcn_update_devices_online_and_total: {
        fcn: null,
        can_continue: true,
        time: 10000
    }
};

function req(path)
{
    let x = new XMLHttpRequest();
    x.open("GET", `http://modularmonitor.local${path}`, false);
    x.send(null);
    return JSON.parse(x.responseText);
}

async function req_async(path) {
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
}

function sec2str(seconds)
{
    const sec = Math.floor(seconds) % 60;
    const min = Math.floor(seconds / 60) % 60;
    const hour = Math.floor(seconds / 3600);

    return hour > 0 ? `${hour}h${min}m${sec}s` :
          (min  > 0 ? `${min}m${sec}s` : `${sec}s`);
}

workers.fcn_update_devices.fcn = async function() {
    const self = workers.fcn_update_devices;
    if (!self.can_continue) return;

    self.can_continue = false;
    try {
        if (self.step == null) { /* configure */
            const j = await req_async("/get_device/count");
            const total = j["count"];
            /* init if needed */
            for(let i = 0; i < total; ++i) {
                if (storage.devices[i] == null) storage.devices[i] = {};
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
                const j = await req_async(`/get_device/${self.step.dev_now}`);

                storage.devices[self.step.dev_now]["online"] = j["online"];
                storage.devices[self.step.dev_now]["has_issues"] = j["has_issues"];
                storage.devices[self.step.dev_now]["last_updated"] = j["last_updated"];
                storage.devices[self.step.dev_now]["data"] = [];
                
                self.step.curr_dev_max_idx = Number(j["max_index"]);
                ++self.step.curr_dev_idx;
            }
            else {
                const j = await req_async(`/get_device/${self.step.dev_now}?index=${self.step.curr_dev_idx}`);
                
                storage.devices[self.step.dev_now]["data"][self.step.curr_dev_idx] = {
                    path: j["path"],
                    values: j["data"]
                };

                if (++self.step.curr_dev_idx >= self.step.curr_dev_max_idx) {
                    self.step.curr_dev_idx = -1;
                    self.step.dev_now = (self.step.dev_now + 1) % self.step.dev_max;
                }
            }
        }
    }
    catch(err) {
        console.log("ERR: " + err);
    }
    self.can_continue = true;
}
workers.fcn_update_time.fcn = async function() {
    const self = workers.fcn_update_time;
    if (!self.can_continue) return;
    self.can_continue = false;

    try {
        const el = document.getElementById("el-date");
        device_time = Number((await req_async("/time"))["time"]) / 1000;
        el.innerText = sec2str(device_time);
    }
    catch(err) {
        console.log("ERR: " + err);
    }
    self.can_continue = true;
}
workers.fcn_update_devices_online_and_total.fcn = function() {
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
        console.log("ERR: " + err);
    }
    self.can_continue = true;
}

function start_timers() {
setInterval(workers.fcn_update_devices.fcn, workers.fcn_update_devices.time);
setInterval(workers.fcn_update_time.fcn, workers.fcn_update_time.time);
setInterval(workers.fcn_update_devices_online_and_total.fcn, workers.fcn_update_devices_online_and_total.time);
}