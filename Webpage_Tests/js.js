function httpGetAsync(u, cb) { 
    let x = new XMLHttpRequest();
    x.onreadystatechange = function() { if (x.readyState == 4 && (x.status >= 200 || x.status < 300)) cb(x.responseText);}
    x.open("GET", u, true); // true for asynchronous 
    x.send(null);
}