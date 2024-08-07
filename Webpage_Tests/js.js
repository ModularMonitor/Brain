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
        time: 100,
        step: null
    },
    fcn_update_time: {
        fcn: null,
        time: 1000
    },
    fcn_update_devices_online_and_total: {
        fcn: null,
        time: 2500
    }
};

function req(path)
{
    let x = new XMLHttpRequest();
    x.open("GET", `http://modularmonitor.local${path}`, false);
    x.send(null);
    return JSON.parse(x.responseText);
}

function sec2str(seconds)
{
    const sec = Math.floor(seconds) % 60;
    const min = Math.floor(seconds / 60) % 60;
    const hour = Math.floor(seconds / 3600);

    return hour > 0 ? `${hour}h${min}m${sec}s` :
          (min  > 0 ? `${min}m${sec}s` : `${sec}s`);
}

workers.fcn_update_devices.fcn = function() {
    const self = workers.fcn_update_devices;

    if (self.step == null) { /* configure */
        const j = req("/get_device/count");
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
            const j = req(`/get_device/${self.step.dev_now}`);

            storage.devices[self.step.dev_now]["online"] = j["online"];
            storage.devices[self.step.dev_now]["has_issues"] = j["has_issues"];
            storage.devices[self.step.dev_now]["last_updated"] = j["last_updated"];
            storage.devices[self.step.dev_now]["data"] = [];
        }
        else {
            const j = req(`/get_device/${self.step.dev_now}?index=${self.step.curr_dev_idx}`);
            
            storage.devices[self.step.dev_now]["data"][self.step.curr_dev_idx] = {
                path: j["path"],
                values: j["data"]
            };

            if (++self.step.curr_dev_idx >= self.step.curr_dev_max_idx) {
                self.step.curr_dev_idx = 0;
                self.step.dev_now = (self.step.dev_now + 1) % self.step.dev_max;
            }
        }
    }

}
workers.fcn_update_time.fcn = function() {
    const el = document.getElementById("el-date");
    device_time = Number(req("/time")["time"]) / 1000;
    el.innerText = sec2str(device_time);
}
workers.fcn_update_devices_online_and_total.fcn = function() {
    const el_on = document.getElementById("el-online");
    const el_tot = document.getElementById("el-total");
    let counter = 0;
    storage.devices.forEach(function(e) { if (e["online"]) ++counter; }); 
    el_on.innerText = `${counter}`;
    el_tot.innerText = `${storage.devices.length}`;
}

function start_timers() {
setInterval(workers.fcn_update_devices.fcn, workers.fcn_update_devices.time);
setInterval(workers.fcn_update_time.fcn, workers.fcn_update_time.time);
setInterval(workers.fcn_update_devices_online_and_total.fcn, workers.fcn_update_devices_online_and_total.time);
}