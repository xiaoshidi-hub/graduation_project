t <html><head><title>温度检测</title>
t <meta http-equiv="Content-Type" content="text/html; charset=	GBK" />
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("ds18b20.cgx", 500);
t function plotADGraph() {
t  adVal = document.getElementById("ad_volts").value;
t  numVal = parseInt(adVal, 16);
t  voltsVal = numVal;
t  tableSize = voltsVal;
t  document.getElementById("ad_table").style.width = (tableSize + '%');
t }
t function periodicUpdateAd() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(formUpdate,plotADGraph);
t   ad_elTime = setTimeout(periodicUpdateAd, formUpdate.period);
t  }
t  else
t   clearTimeout(ad_elTime);
t }
t </script></head>
i pg_header.inc
t <h2 align="center"><br>温度监控</h2>
t <p><font size="3">本页的数据更新支持手动和自动，自动更新是基于JavaScript + XML,
t  从而实现平滑且无闪烁的更新效果。由于DS18B20要外接，这里取消温度显示了。<br><br></font></p>
t <form action="ds18b20.cgi" method="post" name="ad">
t <input type="hidden" value="ad" name="pg">
t <table border=0 width=99%><font size="3">
t <tr style="background-color: #aaccff">
t  <th width=15%>选项</th>
t  <th width=15%>时间</th>
t  <th width=15%>温度</th>
t  <th width=55%>温度图</th></tr>
t <tr><td><img src="pabb.gif">DS18B20:</td>
t   <td align="center">
t <input type="text" readonly style="background-color: transparent; border: 0px"
c g 1  size="10" id="ad_value" value="%0.2d:%0.2d:%0.2d"></td>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c g 2  size="10" id="ad_volts" value="%4.1f C"></td>
t <td height=50><table bgcolor="#FFFFFF" border="2" cellpadding="0" cellspacing="0" width="100%"><tr>
c g 3 <td><table id="ad_table" style="width: %d%%" border="0" cellpadding="0" cellspacing="0">
t <tr><td bgcolor="#0000FF">&nbsp;</td></tr></table></td></tr></table></td></tr>
t </font></table>
t <p align=center>
t <input type=button  value="手动更新" onclick="updateMultiple(formUpdate,plotADGraph)">
t <br><br>自动更新:<input type="checkbox" id="adChkBox" onclick="periodicUpdateAd()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period
