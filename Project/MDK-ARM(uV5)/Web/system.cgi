t <html><head><title>系统设置</title>
t <meta http-equiv="Content-Type" content="text/html; charset=GBK" />
t <script language=JavaScript>
t function changeConfirm(f){
t  if(!confirm('Are you sure you want\nto change the password?')) return;
t  f.submit();
t }
t </script></head>
i pg_header.inc
t <h2 font face="verdana" align=center><br>系统设置</h2>
t <p><font size="3">设置新密码，默认密码，用户名和Realm是在配置文件中设置
t <br><br>
t  这里通过<b>POST</b>方式发生数据给Web server.</font></p>
t <form action=index.htm method=post name=cgi>
t <input type=hidden value="sys" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>选项</th>
t  <th width=60%>设置</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
c d 1 <tr><td><img src=pabb.gif>身份认证</TD><TD><b>%s</b></td></tr>
t <tr><td><img src=pabb.gif>输入新密码</td>
c d 2 <td><input type=password name=pw size=10 maxlength=10 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>确认新密码</td>
c d 2 <td><input type=password name=pw2 size=10 maxlength=10 value="%s"></td></tr>
t </font></table>
# Here begin button definitions
t <p align=center>
t <input type=button name=set value="确定" onclick="changeConfirm(this.form)">
t <input type=reset value="取消">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.
