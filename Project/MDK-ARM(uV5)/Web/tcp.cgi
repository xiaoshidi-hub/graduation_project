t <html><head><title>TCP Socket 状态</title>
t <meta http-equiv="refresh" content="5" charset= GBK></head>
i pg_header.inc
t <h2 align=center><br>TCP Socket 状态，每5秒更新一次页面</h2>
t <center>
t <table border=0 width=800><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=14%>Socket</th>
t  <th width=20%>状态</th>
t  <th width=20%>远程IP</th>
t  <th width=16%>远程端口</th>
t  <th width=16%>本地端口</th>
t  <th width=14%>保持连接溢出时间</th>
t </tr>
c c
t </font></table>
t <form action=tcp.cgi method=post name=form1>
t  <table width=660>
t  <tr><td align="center"><br><br>
t  <input type=button value="手动刷新" onclick="location='/tcp.cgi'">
t  </td></tr></table>
t  </center>
t </form>
i pg_footer.inc
. End of script must be closed with period.

