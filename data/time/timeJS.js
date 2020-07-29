var previouseButton;
var currentPort;
var SERVER_URL = ''

$(function () {
  $.ajax({
    type: 'GET',
    url: SERVER_URL + '/restAPI/port',
    jsonp: false,
    contentType: 'application/json',
    success: function (response) {
      var ports = response.ports;
      for (var i = 0; i < ports.length; i++) {
        var btnId = 'PORT_' + ports[i];
        var btnText = 'Port '+ ports[i].toLocaleUpperCase();
        var element = '<button type="button" class="btn btn-success btn-rounded ports" value="'+ports[i]+'" id="'+btnId+'">'+btnText+'</button>';
        $('#portGroup').append(element);
      }
      $('.ports').click(function(event){
        populateTimeList($(this).val());
        $('#'+previouseButton).button('toggle');
        $(this).button('toggle');
        previouseButton = $(this).attr('id');
        currentPort = $(this).val();
      });
    },
    error: function (e) {
      console.log('error', e);
    }
  });
});

function populateTimeList(value){
  $("#output").find('tr.record').remove();

  $.ajax({
    type: 'GET',
    url: SERVER_URL + '/restAPI/port',
    jsonp: false,
    contentType: 'application/json',
    data: { 
      port_id: value
    },
    success: function (response) {
      var timing = response.data;
      for (var i = 0; i < timing.length; i++) {
        var pickerId = "timePicker" + i;
        var row = '<tr class="record">';
        row += '<td class="d-none">';
        row += value;
        row += "</td>";
        row += '<td class="py-2-half"><input id="' + pickerId + '" class="form-control border-0" />';
        row += "</td>";
        row += '<td class="py-2-half" contenteditable="true">';
        row += timing[i].duration;
        row += "</td>";
        row += '<td>';
        row += '<span class="table-remove px-2"><button type="button" class="btn btn-danger btn-rounded btn-sm my-0">X</button></span>';
        row += "</td>";
        row += "</tr>";

        $('#output tbody').append(row);
        $('#' + pickerId).timepicker({
          timeFormat: 'G:i',
          minTime: '05:00',
          maxTime: '23:45',
          step: 15
        });
        var d = new Date();
        var timeArray = timing[i].time.split(":");
        d.setHours(timeArray[0], 0);
        d.setMinutes(timeArray[1], 0);
        $('#' + pickerId).timepicker('setTime', d);
      }
    },
    error: function (e) {
      console.log('error', e);
    }
  });
}

$(function () {
  $('#btnCancel').click(function (event){
    populateTimeList(currentPort);
  });
});

$(function () {
  $('.btn-switch').click(function(event){
    $(this).button('toggle');
    btnOption = $(this).attr('Value');
    //currentPort = $(this).val();

    $.ajax({
      type: 'GET',
      url: SERVER_URL + '/'+btnOption+'?port_id='+currentPort,
      jsonp: false,
      contentType: 'application/json',
      //dataType: 'json', //specify jsonp
      success: function (response) {
        null;
      },
      error: function (response) {
        alert("Switching port failed");
      }
    });
  });
});

// A few jQuery helpers for exporting only
jQuery.fn.pop = [].pop;
jQuery.fn.shift = [].shift;

$(function () {
  $('#btnSave').click(function (event){
    const $rows = $("#output").find('tr:not(:hidden)');
    const headers = [];
    const data = [];
    const timeData = {};
    var port_id;

    // Get the headers (add special header logic here)
    $($rows.shift()).find('th:not(:empty)').each(function () {
      headers.push($(this).text().toLowerCase());
    });

    // Turn all existing rows into a loopable array
    $rows.each(function () {
      const $td = $(this).find('td');
      const h = {};

      // Use the headers from earlier to name our hash keys
      headers.forEach((header, i) => {
        if (header == "time"){
          const $time = $td.eq(i).find('input');
          h[header] = $time.val();
        }
        else if (header == "duration"){          
          h[header] = $td.eq(i).text();
        }
        else if (header == "port_id"){          
          port_id = $td.eq(i).text();
        }
      });

      data.push(h);
    });

    timeData["port_id"] = port_id;
    timeData["data"] = data;
    
    // Output the result
    // $('#debugSpan').val(JSON.stringify(timeData));

    $.ajax({
      type: 'POST',
      url: SERVER_URL + '/restAPI/port',
      data: JSON.stringify(timeData),
      jsonp: false,
      contentType: 'application/json',
      //dataType: 'json', //specify jsonp
      success: function (response) {
        $('#success').toast('show');
      },
      error: function (response) {
        $('#failed').toast('show');
      }
    });
  });
});


$(function () {
  $('#btnOk1').click(function (event) {
    const $rows = $("#output").find('tr:not(:hidden)');
    const headers = [];
    const data = [];

    // Get the headers (add special header logic here)
    $($rows.shift()).find('th:not(:empty)').each(function () {

      headers.push($(this).text().toLowerCase());
    });

    // Turn all existing rows into a loopable array
    $rows.each(function () {
      const $td = $(this).find('td');
      const h = {};

      // Use the headers from earlier to name our hash keys
      headers.forEach((header, i) => {
        if (header != "time")
          h[header] = $td.eq(i).text();
        else {
          const $time = $td.eq(i).find('input');
          h[header] = $time.val();
        }
      });

      data.push(h);

      // Output the result
      $('#tstSpan').val(JSON.stringify(data));
    });


    $.ajax({
      type: 'POST',
      url: 'http://dummy.restapiexample.com/api/v1/create',
      data: JSON.stringify(data),
      jsonp: false,
      contentType: 'application/json',
      dataType: 'json', //specify jsonp
      success: function (response) {
        //alert('Save success');
      },
      error: function (response) {
        alert('Error');
      }
    });
  });
});

