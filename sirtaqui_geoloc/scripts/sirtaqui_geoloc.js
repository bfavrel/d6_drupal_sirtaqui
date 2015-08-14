
$(document).ready(function(){

    if(navigator.geolocation && $(window).width() < 980) {
        var watchPositionID = navigator.geolocation.watchPosition(positionCallback, errorCallback, {
            timeout: 30000, 
            maximumAge: 20000,
            enableHighAccuracy: true
        });
    }

    function positionCallback(position) {

        $.cookie('sirtaqui_geoloc_longitude', position.coords.longitude);
        $.cookie('sirtaqui_geoloc_latitude', position.coords.latitude);
    }

    function errorCallback(error) {
        console.log(error);
        navigator.geolocation.clearWatch(watchPositionID);
    }
});