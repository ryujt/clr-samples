const readline = require("readline");
const udp = require('dgram');

const datas = new Map();
const server = udp.createSocket('udp4');

server.on('error', function (error) {
    console.log('Error: ' + error);
    server.close();
});

function printNonDuplicateMessages(msg) {
    try {
        let data = msg.toString();
        if (!datas.has(data)) {
            console.log(data);
            datas.set(data, "");
        }
    } catch (error) {
    }
}

function printDuplicateTraceIds(msg) {
    try {
        let data = JSON.parse(msg.toString());
        if (datas.has(data.TraceId)) {
            console.log("Duplicate TraceId: " + data.TraceId);
        } else {
            datas.set(data.TraceId, data);
        }
    } catch (error) {
    }
}

server.on('message', function (msg) {
    if (process.argv[2] === 'nd') {
        printNonDuplicateMessages(msg);
    } else if (process.argv[2] === 'dt') {
        printDuplicateTraceIds(msg);
    } else {
        console.log(msg.toString());
    }
});

server.on('listening', function () {
    var address = server.address();
    var port = address.port;
    var family = address.family;
    var ipaddr = address.address;
    console.log('Server is listening at port' + port);
    console.log('Server ip :' + ipaddr);
    console.log('Server is IP4/IP6 : ' + family);
});

server.on('close', function () {
    console.log('Socket is closed !');
});

server.bind(2222);

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

rl.question("", function (message) {
    console.log(`TraceId: ${message}`);
    datas.forEach(e => {
        if (e.TraceId == message) console.log(e);
    });
    console.log("************************************************************************");
});
