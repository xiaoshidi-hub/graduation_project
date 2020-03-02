t <html><head><title>蜂鸣器控制</title>
t <meta http-equiv="Content-Type" content="text/html; charset=	GBK" />
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("beep.cgx", 300);
t var formUpdate1 = new periodicObj("beep1.cgx", 300);
t </script></head>
i pg_header.inc
t <h2 align=center><br>蜂鸣器控制</h2>
t <p><font size="3">蜂鸣器的控制采用JavaScript + XML,
t 从而点击按钮控制的时候实现平滑且无闪烁的更新效果。<br><br></font></p>
t <form action=beep.cgi method=post name=form1>
t <input type=hidden value="led" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>选项</th>
t  <th width=60%>设置</th></tr>
t <tr><td><img src=pabb.gif>蜂鸣器控制:</td>
t <td><table><tr valign="middle">
# Here begin the 'button' definitions
t <td><input type="button" id="Btn1" value="蜂鸣器响两次" onclick="updateMultiple(formUpdate1)"></td>
t <td><input type="button" id="Btn0" value="蜂鸣器响一次" onclick="updateMultiple(formUpdate)"></td>
t </font></table></td></tr>
t </table></form>
i pg_footer.inc
. End of script must be closed with period.


