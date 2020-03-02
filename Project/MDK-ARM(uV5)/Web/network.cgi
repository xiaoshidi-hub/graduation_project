t <html><head><title>网络设置</title>
t <meta http-equiv="Content-Type" content="text/html; charset=	GBK" />
t <script language=JavaScript>
t function changeConfirm(f){
t  if(!confirm('Are you sure you want to change\nthe Network settings?')) return;
t  f.submit();
t }
t </script></head>
i pg_header.inc
t <h2 font face="verdana" align=center><br>网络设置</h2>
t <p><font size="3">用户可以在这里给Web Server设置新的IP地址，
t 设置了新的IP后，浏览器就得要访问新设置的IP地址。设置过程中要当心，
t 有时候会造成永远无法访问，只能通过硬件复位解决。默认情况下是使能了
t DHCP的，Web Swrver会自动从路由器获取一个IP地址，手动设置容易IP
t 冲突。从而造成设置失败。<br><br>
t 这里通过<b>GET</b> 方式发数据给 Web server.</font></p>
t <form action=network.cgi method=get name=cgi>
t <input type=hidden value="net" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>选项</th>
t  <th width=60%>设置</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <tr><td><img src=pabb.gif>IP 地址</td>
c a i <td><input type=text name=ip value="%d.%d.%d.%d" size=18 maxlength=18></td></tr>
t <tr><td><IMG SRC=pabb.gif>子网掩码</td>
c a m <td><input type=text name=msk value="%d.%d.%d.%d" size=18 maxlength=18></td></tr>
t <tr><td><IMG SRC=pabb.gif>默认网关</td>
c a g <td><input type=text name=gw value="%d.%d.%d.%d" size=18 maxlength=18></td></tr>
t <tr><td><IMG SRC=pabb.gif>首选DNS服务器</td>
c a p <td><input type=text name=pdns value="%d.%d.%d.%d" size=18 maxlength=18></td></tr>
t <tr><td><IMG SRC=pabb.gif>备用DNS服务器</td>
c a s <td><input type=text name=sdns value="%d.%d.%d.%d" size=18 maxlength=18></td></tr>
t </font></table>
# Here begin button definitions
t <p align=center>
t <input type=button name=set value="确认" onclick="changeConfirm(this.form)">
t <input type=reset value="取消">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.

