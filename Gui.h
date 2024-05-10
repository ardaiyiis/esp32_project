

// note R"KEYWORD( html page code )KEYWORD"; 
// again I hate strings, so char is it and this method let's us write naturally

const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en" class="js-focus-visible">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
<title>Web Page Update Demo</title>
</head>

  <style>
    body {
      margin: 0;
      text-align: center;
    }
    .ulclass {
      position: relative;
      left: -20px;  
    }
    
    ul.topnav {
      list-style-type: none;
      margin: 0;
      padding: 0;
      overflow: hidden;
      background-color: #333;
    }
    
    ul.topnav li {float: left;
    font-family: "Verdana", "Arial", sans-serif;
    }
    
    ul.topnav li a {
      font-size: 20px;
      display: block;
      color: white;
      text-align: center;
      padding: 14px 16px;
      text-decoration: none;
    }
    
    ul.topnav li a:hover:not(.active) {background-color: #111;}
    
    ul.topnav li a.active {
      background-color: #04aa6d;
      font-weight: bald;
    }
    
    ul.topnav li.right {float: right;}
    
    @media screen and (max-width: 600px) {
      ul.topnav li.right, 
      ul.topnav li {float: none;}
    }
 
    .panrpmslider {
      width: 80%;
      height: 55px;
      outline: none;
      height: 25px;
    }
    .tiltrpmslider {
      width: 80%;
      height: 55px;
      outline: none;
      height: 25px;
    }
    .bodytext {
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 24px;
      text-align: left;
      font-weight: light;
      border-radius: 5px;
      display:inline;
    }
    
    .category {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: bold;
      font-size: 32px;
      line-height: 50px;
      padding: 20px 10px 0px 10px;
      color: #000000;
    }
    .heading {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: normal;
      font-size: 28px;
      text-align: left;
    }
  
    .btn {
      border: none;
      background-color: black;
      color: white;
      padding: 11px 22px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      margin: 4px 2px;
      border-radius: 8px;
    }

    .btn:active {
      background-color: #3e8e41;
      box-shadow: 0 5px #666;
      transform: translateY(4px);
    }

    input[type=text] {
      width: 130px;
      box-sizing: border-box;
      border: 2px solid #ccc;
      border-radius: 4px;
      font-size: 16px;
      background-color: white;
      background-position: 10px 10px; 
      background-repeat: no-repeat;
      padding: 12px 20px 12px 40px;
      transition: width 0.4s ease-in-out;
     }
      
    .foot {
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 12px;
      position: relative;
      height:   30px;
      text-align: center;   
      color: #AAAAAA;
      line-height: 20px;
    }
    .container {
      max-width: 1800px;
      margin: 0 auto;
    }
    table tr:first-child th:first-child {
      border-top-left-radius: 5px;
    }
    table tr:first-child th:last-child {
      border-top-right-radius: 5px;
    }
    table tr:last-child td:first-child {
      border-bottom-left-radius: 5px;
    }
    table tr:last-child td:last-child {
      border-bottom-right-radius: 5px;
    }
    
  </style>

  <body style="background-color: #efefef" onload="process()">
  
    <header>

      <ul class="topnav">
        <li><a class="active">DEVICE</a></li>
        <li><a class="navdata" id = "date">mm/dd/yyyy</a></li>
        <li><a class="navdata" id = "time">00:00:00</a></li>
      </ul>
    </header>
  
    <main class="container" style="padding:0 16px;">

    <div class="category">Kontroller</div>
    <br>
    <div class="bodytext"> LAZER </div>
    <button type="button" class = "btn" id = "btn0" onclick="ButtonPress0()">Toggle</button>
    </div>
    <br>
    <div class="bodytext">HAREKET</div>
    <button type="button" class = "btn" id = "btn1" onclick="ButtonPress1()">Toggle</button>
    </div>
    <br>
    <br>
    <div class="bodytext">Yatay (Değer: <span id="panrpm"></span>)</div>
    <br>
    <input type="range" class="panrpmslider" min="0" max="180" value = "0" width = "0%" oninput="UpdateSlider_0(this.value)"/>
    <br>
    <br>
    <div class="bodytext">Dikey (Değer: <span id="tiltrpm"></span>)</div>
    <br>
    <input type="range" class="tiltrpmslider" min="0" max="110" value = "0" width = "0%" oninput="UpdateSlider_1(this.value)"/>
    <br>
    <br>
    <div class="category">Profiller</div>
    <ul class="ulclass" style="list-style-type:none;">
      <li>
          <input type="text" id="panProfile_0" name="panProfile_0" placeholder="Yatay"/>
          <input type="text" id="tiltProfile_0" name="tiltProfile_0" placeholder="Dikey"/>
          <button id="0" class = "btn" onclick="UpdateForms('0')">Kaydet</button>        
      </li>
      <br>
      <li>
          <input type="text" id="panProfile_1" name="panProfile_1" placeholder="Yatay"/>
          <input type="text" id="tiltProfile_1" name="tiltProfile_1" placeholder="Dikey"/>
          <button id="1" class = "btn" onclick="UpdateForms('1')">Kaydet</button> 
      </li>
      <br>
      <li>
          <input type="text" id="panProfile_2" name="panProfile_2" placeholder="Yatay"/>
          <input type="text" id="tiltProfile_2" name="tiltProfile_2" placeholder="Dikey"/>
          <button id="2" class = "btn" onclick="UpdateForms('2')">Kaydet</button> 
      </li>
      <br>
      <li>
          <input type="text" id="panProfile_3" name="panProfile_3" placeholder="Yatay"/>
          <input type="text" id="tiltProfile_3" name="tiltProfile_3" placeholder="Dikey"/>
          <button id="3" class = "btn" onclick="UpdateForms('3')">Kaydet</button> 
      </li>
    </ul>
  </main>

  <footer div class="foot" id = "temp" > LICENSE TEXT </div></footer>
  
  </body>


  <script type = "text/javascript">
  
    // global variable visible to all java functions
    var xmlHttp=createXmlHttpObject();

    // function to create XML object
    function createXmlHttpObject(){
      if(window.XMLHttpRequest){
        xmlHttp=new XMLHttpRequest();
      }
      else{
        xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
      }
      return xmlHttp;
    }

    // function to handle button press from HTML code above
    // and send a processing string back to server
    // this processing string is use in the .on method
    function ButtonPress0() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      // if you want to handle an immediate reply (like status from the ESP
      // handling of the button press use this code
      // since this button status from the ESP is in the main XML code
      // we don't need this
      // remember that if you want immediate processing feedbac you must send it
      // in the ESP handling function and here
      /*
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          message = this.responseText;
          // update some HTML data
        }
      }
      */
       
      xhttp.open("PUT", "BUTTON_0", false);
      xhttp.send();
    }


    // function to handle button press from HTML code above
    // and send a processing string back to server
    // this processing string is use in the .on method
    function ButtonPress1() {
      var xhttp = new XMLHttpRequest(); 
      /*
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("button1").innerHTML = this.responseText;
        }
      }
      */
      xhttp.open("PUT", "BUTTON_1", false);
      xhttp.send(); 
    }
    
    function UpdateSlider_0(value) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          // update the web based on reply from  ESP
          document.getElementById("panrpm").innerHTML=this.responseText;
        }
      }
      // this syntax is really weird the ? is a delimiter
      // first arg UPDATE_SLIDER is use in the .on method
      // server.on("/UPDATE_SLIDER", UpdateSlider);
      // then the second arg VALUE is use in the processing function
      // String t_state = server.arg("VALUE");
      // then + the controls value property
      xhttp.open("PUT", "UPDATE_SLIDER_0?VALUE="+value, true);
      xhttp.send();
    }

    function UpdateSlider_1(value) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {        
          document.getElementById("tiltrpm").innerHTML=this.responseText;
        }
      }
      xhttp.open("PUT", "UPDATE_SLIDER_1?TILTER="+value, true);
      xhttp.send();
    }

    function UpdateForms(identity) {
      var xhttp = new XMLHttpRequest();
      if (identity == "0"){            
      var pan = document.getElementById('panProfile_0').value;
      var tilt  = document.getElementById('tiltProfile_0').value;
      var PanTiltIdentity = pan +','+ tilt +','+ identity;
      }
      else if (identity == "1"){            
      var pan = document.getElementById('panProfile_1').value;
      var tilt  = document.getElementById('tiltProfile_1').value;
      var PanTiltIdentity = pan +','+ tilt +','+ identity;
      }
      else if (identity == "2"){            
      var pan = document.getElementById('panProfile_2').value;
      var tilt  = document.getElementById('tiltProfile_2').value;
      var PanTiltIdentity = pan +','+ tilt +','+ identity;
      }
      else if (identity == "3"){            
      var pan = document.getElementById('panProfile_3').value;
      var tilt  = document.getElementById('tiltProfile_3').value;
      var PanTiltIdentity = pan +','+ tilt +','+ identity;
      }
      xhttp.open("PUT", "UPDATE_FORMS?PANTILTIDENTITY="+PanTiltIdentity, true);
      xhttp.send();
    }
    
    // function to handle the response from the ESP
    function response(){
      var message;
      var barwidth;
      var currentsensor;                                                                                                                                                                                        
      var xmlResponse;
      var xmldoc;
      var dt = new Date();
      var color = "#e8e8e8";
     
      // get the xml stream
      xmlResponse=xmlHttp.responseXML;
  
      // get host date and time
      document.getElementById("time").innerHTML = dt.toLocaleTimeString();
      document.getElementById("date").innerHTML = dt.toLocaleDateString();
  
     
  
      xmldoc = xmlResponse.getElementsByTagName("LAZER");
      message = xmldoc[0].firstChild.nodeValue;
  
      if (message == 0){
        document.getElementById("btn0").innerHTML="Aç";
        document.getElementById("btn0").style.color="#08d665";
      }
      else{
        document.getElementById("btn0").innerHTML="Kapat";
        document.getElementById("btn0").style.color="#ffffff";
      }         
      xmldoc = xmlResponse.getElementsByTagName("HAREKET");
      message = xmldoc[0].firstChild.nodeValue;

      if (message == 0){        
        document.getElementById("btn1").innerHTML="Aç";
        document.getElementById("btn1").style.color="#08d665";
      }
      else {
        document.getElementById("btn1").innerHTML="Kapat";
        document.getElementById("btn1").style.color="#ffffff";
      }
     }
  
    // general processing code for the web page to ask for an XML steam
    // this is actually why you need to keep sending data to the page to 
    // force this call with stuff like this
    // server.on("/xml", SendXML);
    // otherwise the page will not request XML from the ESP, and updates will not happen
    function process(){
     
     if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {
        xmlHttp.open("PUT","xml",true);
        xmlHttp.onreadystatechange=response;
        xmlHttp.send(null);
      }       
        // you may have to play with this value, big pages need more porcessing time, and hence
        // a longer timeout
        setTimeout("process()",100);
    }
  
  
  </script>

</html>



)=====";
