const devices = [];
let device_time = 0; // seconds
const http_tries = 3;

function makeRequest(method, url) {
    return new Promise(function (resolve, reject) {
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
                statusText: xhr.statusText
            });
        };
        xhr.send();
    });
}

async function httpGetAsync(u) {
    for(let tries = 0; tries < http_tries; ++tries) {
        try {
            return await makeRequest("GET", u);
        }
        catch(err) {
            console.log(`Got exception on XMLHttpRequest: ${err}. [${tries + 1} / ${http_tries}]`);
        }
    }
    return 
}

function secondsToString(seconds)
{
    //const ms = Math.round(seconds * 1000) % 1000;
    const sec = Math.round(seconds) % 60;
    const min = Math.round(seconds / 60) % 60;
    const hour = Math.round(seconds / 3600);

    return hour > 0 ? `${hour}h${min}m${sec}s` :
          (min  > 0 ? `${min}m${sec}s` : `${sec}s`);
}



// Get all up to date info
async function _update_all_of_em()
{
    const j = JSON.parse(await httpGetAsync("/get_device/count"));

    console.log(`[LG] Got device count: ${j["count"]}`);

    for(let i = 0; i < j["count"]; ++i) {
        const recipe = JSON.parse(await httpGetAsync(`/get_device/${i}`));

        devices[i] = {
            online: recipe["online"],
            has_issues: recipe["has_issues"],
            last_updated: recipe["last_updated"]
        };

        console.log(`[LG] Got device ${i} ${recipe["online"] ? "ONLINE" : "OFFLINE"}`);

        devices[i]["data"] = [];

        for(let k = 0; k < recipe["max_index"]; ++k) {

            console.log(`[LG] Reading device ${i} prop ${k} of ${recipe["max_index"]}...`);

            const each = JSON.parse(await httpGetAsync(`/get_device/${i}?index=${k}`));
            devices[i].data[k] = {
                path: each["path"],
                data: each["data"]
            };
        }
    }
}

// Update time it has been awake
async function _update_el_date()
{
    const el = document.getElementById("el-date");
    const j = JSON.parse(await httpGetAsync("/time"));
    device_time = Number(j["time"]) / 1000;
    el.innerText = secondsToString(device_time);
}

async function _update_devices_online()
{
    const el = document.getElementById("el-online");
    let counter = 0;
    devices.forEach(function(e) { if (e.online) ++counter; });
 
    el.innerText = `${counter}`;
}


/*setTimeout(_update_all_of_em, 500);
setInterval(_update_all_of_em, 5000);
setInterval(_update_el_date, 1000);
setInterval(_update_devices_online, 5000);*/