
$(document).ready (function () {
	$("#toggle_dev").on ('click', function (e) {
		e.preventDefault ();
		$("#dev").toggleClass ("max");
	});

	$("#view_race").on ('click', '#reset', function (e) {
		e.preventDefault ();
		clearNames ();
		clearTimes ();
	}).on ('click', '#rotate', function (e) {
		e.preventDefault ();
		clearTimes ();
		rotateNames ();
	}).on ('click', '#start', function (e) {
		e.preventDefault ();
		sendEvent('start_click', { });
	}).on ('click', '#stop', function (e) {
		e.preventDefault ();
		sendEvent('stop_click', { });
	});
	connect ();
});

var socket;

function sendEvent (evt, params) {
	socket.send (JSON.stringify ({
		'event': evt,
		params: params
	}));
}

function processMessage (data) {
	switch (data.type) {
		case 'race_state':
			updateRaceState (data.state);
			//	fall-through
		case 'socket_state':
		case 'serial_state':
			$("#" + data.type).text (data.state);
			break;
		case 'finish':
			recordFinish (data);
			break;
		case 'debug':
			$("#debug").append (data.message + "\n");
			break;
		default:
			console.log ('Unknown message type: ' + data.type);
			break;
	}
}

function connect () {
	socket = new WebSocket("ws://localhost:7777");

	socket.onmessage = function (evt) {
		processMessage (JSON.parse (evt.data));
	};
}

function updateRaceState (state) {
	$('body').attr ('class', state);
	switch (state) {
		case 'Calibrating':
			clearTimes ();
			break;
	}
}

function raceState () {
	return $('body').attr ('class');
}

function recordFinish (data) {
	$row = $("#view_race").find ('tr.lane_' + data.lane);
	$row.find ('.place').text ('').append ($('<span/>', { text: data.place }));
	$row.find ('.time').text (data.time + 's');

	var diff = data.diff;
	if (diff) {
		diff = '+' + diff.toFixed(2);
	} else {
		diff = '&nbsp';
	}
	$row.find ('.diff').html (diff);
}

function clearTimes () {
	$("#view_race").find ('td.time, td.place, td.diff').text ('');
}
function clearNames () {
	$("#view_race").find ('td.racer input').val ('');
}
function rotateNames () {
	var last = '';
	var tmp = null;
	$inputs = $("#view_race").find ('td.racer input');
	$inputs.each (function () {
		tmp = $(this).val ();
		$(this).val (last);
		last = tmp;
	});
}
