function httpGetAsync(u, cb) { 
    let x = new XMLHttpRequest();
    x.onreadystatechange = function() {         
        if (x.readyState == 4 && (x.status >= 200 || x.status < 300)) {
            cb(x.responseText);
        }
    };
    x.onerror = function(err) { console.log("Error on HTTP request '" + u + "': " + err);}
    x.onabort = function(err) { console.log("Error on HTTP request '" + u + "': " + err);}
    x.open("GET", u, true);
    x.send(null);
}

function update_el_date()
{
    const el = document.getElementById("el-date");    
    httpGetAsync("/time", function(jraw) {
        const j = JSON.parse(jraw);
        el.innerText = Number(j["time"]) / 1000;
    });
}

function _get_all_of_device(device_id)
{
    
}


/*setInterval(update_el_date, 1000);*/