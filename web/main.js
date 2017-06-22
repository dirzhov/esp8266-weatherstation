/*! main.js
version : 1.0.0
authors : Dmitry Irzhov
license : MIT */
$(document).ready(function(){
    var loc = window.location;
    window.host = loc.protocol!=''?(loc.protocol+'//'):'';
    window.host = loc.host!=''?(window.host+loc.host):'';
    window.host = loc.port!=''?(window.host+':'+loc.port):window.host;
    
    if (window.host == '')
        window.host = 'http://192.168.1.85/';
	else
		window.host += '/';

    window.chartColors = {
        red: 'rgb(255,99,132)',
        orange: 'rgb(255,159,64)',
        yellow: 'rgb(255,205,86)',
        green: 'rgb(75,192,192)',
        blue: 'rgb(54,162,235)',
        purple: 'rgb(153,102,255)',
        grey: 'rgb(201,203,207)'
    };

    var color = Chart.helpers.color
        ,timeFormat = 'DD HH:mm:ss'
        ,dateTimeFormat = 'MM/DD/YY HH:mm:ss'
        ,refreshInterval = 5;

    var config = {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: "Temperature(C)",
                fill: true,
                backgroundColor: color(window.chartColors.blue).alpha(0.2).rgbString(),
                borderColor: window.chartColors.blue,
                data: [],
            }, {
                label: "Temperature(F)",
                fill: true,
                backgroundColor: color(window.chartColors.green).alpha(0.2).rgbString(),
                borderColor: window.chartColors.green,
                /*borderDash: [5, 5],*/
                data: [],
                hidden: true
            }, {
                label: "Humidity(%)",
                fill: true,
                backgroundColor: color(window.chartColors.red).alpha(0.2).rgbString(),
                borderColor: window.chartColors.red,
                data: [],
            }, {
                label: "Light(%)",
                fill: true,
                backgroundColor: color(window.chartColors.yellow).alpha(0.2).rgbString(),
                borderColor: window.chartColors.yellow,
                data: [],
            }]
        },
        options: {
            responsive: true,
/*            title:{
                display:true,
                text:'Current values'
            },*/
            tooltips: {
                mode: 'index',
                intersect: false,
            },
            hover: {
                mode: 'nearest',
                intersect: true
            },
            scales: {
                xAxes: [{
                    type: "time",
                    time: {
                        format: timeFormat,
                        // round: 'day'
                        tooltipFormat: 'll HH:mm'
                    },
                    scaleLabel: {
                        display: true,
                        labelString: 'Date'
                    }
                }],
                yAxes: [{
                    display: true,
                    scaleLabel: {
                        display: true,
                        labelString: 'Value'
                    }
                }]
            }
        }
    };
    var confLight = {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: "Light(%)",
                fill: true,
                backgroundColor: color(window.chartColors.blue).alpha(0.2).rgbString(),
                borderColor: window.chartColors.blue,
                data: [],
            }]
        },
        options: {
            responsive: true,
            tooltips: {
                mode: 'index',
                intersect: false,
            },
            hover: {
                mode: 'nearest',
                intersect: true
            },
            scales: {
                xAxes: [{
                    type: "time",
                    time: {
                        format: timeFormat,
                        // round: 'day'
                        tooltipFormat: 'll HH:mm'
                    },
                    scaleLabel: {
                        display: true,
                        labelString: 'Date'
                    }
                }],
                yAxes: [{
                    display: true,
                    scaleLabel: {
                        display: true,
                        labelString: 'Value(%)'
                    }
                }]
            }
        }
    };
    var wmconfig = {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: "Temperature(C)",
                fill: true,
                backgroundColor: color(window.chartColors.blue).alpha(0.2).rgbString(),
                borderColor: window.chartColors.blue,
                data: [],
            }, {
                label: "Temperature(F)",
                fill: true,
                backgroundColor: color(window.chartColors.green).alpha(0.2).rgbString(),
                borderColor: window.chartColors.green,
                /*borderDash: [5, 5],*/
                data: [],
                hidden: true
            }, {
                label: "Humidity(%)",
                fill: true,
                backgroundColor: color(window.chartColors.red).alpha(0.2).rgbString(),
                borderColor: window.chartColors.red,
                data: [],
            }, {
                label: "Light(%)",
                fill: true,
                backgroundColor: color(window.chartColors.yellow).alpha(0.2).rgbString(),
                borderColor: window.chartColors.yellow,
                data: [],
            }]
        },
        options: {
            responsive: true,
            tooltips: {
                mode: 'index',
                intersect: false,
            },
            hover: {
                mode: 'nearest',
                intersect: true
            },
            scales: {
                xAxes: [{
                    type: "time",
                    time: {
                        format: dateTimeFormat,
                        // round: 'day'
                        tooltipFormat: 'll HH:mm'
                    },
                    scaleLabel: {
                        display: true,
                        labelString: 'Date'
                    }
                }],
                yAxes: [{
                    display: true,
                    scaleLabel: {
                        display: true,
                        labelString: 'Value'
                    }
                }]
            }
        }
    };


    var ctx = document.getElementById("tempChart");
    ctx.width = $(ctx).parent().width();
    ctx.height = $(ctx).parent().height();
    var tempChart = new Chart(ctx, config);

    var ctx2 = document.getElementById("lightChart");
    ctx2.width = $(ctx2).parent().width();
    ctx2.height = $(ctx2).parent().height();
    var lightChart = new Chart(ctx2, confLight);

    var ctx3 = document.getElementById("weekChart");
    ctx3.width = $(ctx3).parent().width();
    ctx3.height = $(ctx3).parent().height();
    var weekChart = new Chart(ctx3, wmconfig);

    var ctx4 = document.getElementById("monthChart");
    ctx4.width = $(ctx4).parent().width();
    ctx4.height = $(ctx4).parent().height();
    var monthChart = new Chart(ctx4, wmconfig);

        
    var getV = function(index, data) {
        switch (index) {
            case 0: return parseFloat(data.c); 
                break;
            case 1: return parseFloat(data.c*1.8+32); 
                break;
            case 2: return parseFloat(data.h); 
                break;
            case 3: return parseFloat(data.l/1024*100); 
                break;
        }
        return NaN;
    }

    var readTemp = function() {
        $.ajax({
            type: 'GET',
            url: host+'temp',
            dataType: 'json',
            success: function(data){
                if (config.data.datasets.length <= 0)
                    return;

                if (config.data.labels.length > 50) {
                    for (var index = 0; index < config.data.datasets.length; ++index) {
                        config.data.datasets[index].data.shift();
                    }
                    config.data.labels.shift();
                }

                var lastTime = moment()
                    , newTime = lastTime
					.clone()
					.add(refreshInterval, 's')
					.format(timeFormat);
                
				config.data.labels.push(newTime);

                var v;
                
				for (var index = 0; index < config.data.datasets.length; ++index) {
                    config.data.datasets[index].data.push({
                        x: newTime,
                        y: getV(index, data),
                    });
				}

				tempChart.update();
            },
            error: function(xhr,type) {
                console.log("%s %s" ,xhr,type)
            }
        });
    }

    var readLight = function() {
        $.ajax({
            type: 'GET',
            url: host+'light',
            dataType: 'json',
            success: function(data){
                if (confLight.data.labels.length > 50) {
                    confLight.data.datasets[0].data.shift();
                    confLight.data.labels.shift();
                }

                var lastTime = moment()
                    , newTime = lastTime
					.clone()
					.add(refreshInterval, 's')
					.format(timeFormat);
                
				confLight.data.labels.push(newTime);

                confLight.data.datasets[0].data.push({
                    x: newTime,
                    y: parseFloat(data.v),
                });

				lightChart.update();
            },
            error: function(xhr,type) {
                console.log("%s %s" ,xhr,type)
            }
        });
    }

    var readData = function(chart, call) {
        $.ajax({
            type: 'GET',
            url: host+call,
            dataType: 'json',
            success: function(data){
                wmconfig.data.labels = [];
                for (var index = 0; index < wmconfig.data.datasets.length; ++index)
                    wmconfig.data.datasets[index].data = [];
                
                var lastTime = moment().subtract(data.d, 'milliseconds') // minus duration from esp started
                    , newTime = lastTime.clone();
                
                for (var i=0; i<data.v.length; i++) {
                    newTime = newTime.add(data.i, data.iu) // i - interval, iu - interval unit

                    wmconfig.data.labels.push(newTime);

                    for (var index = 0; index < wmconfig.data.datasets.length; ++index) {
                        wmconfig.data.datasets[index].data.push({
                            x: newTime.format(dateTimeFormat),
                            y: getV(index, data.v[i]),
                        });
                    }
                }

				chart.update();
            },
            error: function(xhr,type) {
                console.log("%s %s" ,xhr,type)
            }
        });
    }


    window.timerTempId = setInterval(readTemp, refreshInterval * 1000);
    window.timerLightId = setInterval(readLight, refreshInterval * 1000);

    var clear = function(chart, config) {
        config.data.labels = [];

        config.data.datasets.forEach(function(dataset, datasetIndex) {
            while (dataset.data.length>0)
                dataset.data.pop();
        });

        chart.update();
    }

    $('#clear_temp').click(function(){
        clear(tempChart, config);
    });
    $('#clear_light').click(function(){
        clear(lightChart, confLight);
    });
    
    $('.tabscontent').tabbedContent({
        links: 'a.r-tabs-anchor',
        onSwitch: function(tab, api) {
            if (tab=='#tab-1') {
                tempChart.update();
            } else if (tab=='#tab-2') {
                lightChart.update();
            } else if (tab=='#tab-3') {
                readData(weekChart, 'week');
            } else if (tab=='#tab-4') {
                readData(monthChart, 'month');
            }
        }
    });
    
    $.getJSON(host+'connectionInfo', function(data, status){
        if (data==null) return;
        $('#ip').text(data.ip);
        $('#ssid').text(data.ssid);
    })
    
});

/*
;(function ($) { 
})($ || Zepto);
*/