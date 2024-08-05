function httpGetAsync(u, cb) { 
    let x = new XMLHttpRequest();
    x.onreadystatechange = function() { if (x.readyState == 4 && (x.status >= 200 || x.status < 300)) cb(x.responseText);}
    x.open("GET", u, true); // true for asynchronous 
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


setInterval(update_el_date, 1000);