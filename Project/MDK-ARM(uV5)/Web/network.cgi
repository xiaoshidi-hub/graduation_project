t <html><head><title>��������</title>
t <meta http-equiv="Content-Type" content="text/html; charset=	GBK" />
t <script language=JavaScript>
t function changeConfirm(f){
t  if(!confirm('Are you sure you want to change\nthe Network settings?')) return;
t  f.submit();
t }
t </script></head>
i pg_header.inc
t <h2 font face="verdana" align=center><br>��������</h2>
t <p><font size="3">�û������������Web Server�����µ�IP��ַ��
t �������µ�IP��������͵�Ҫ���������õ�IP��ַ�����ù�����Ҫ���ģ�
t ��ʱ��������Զ�޷����ʣ�ֻ��ͨ��Ӳ����λ�����Ĭ���������ʹ����
t DHCP�ģ�Web Swrver���Զ���·������ȡһ��IP��ַ���ֶ���������IP
t ��ͻ���Ӷ��������ʧ�ܡ�<br><br>
t ����ͨ��<b>GET</b> ��ʽ�����ݸ� Web server.</font></p>
t <form action=network.cgi method=get name=cgi>
t <input type=hidden value="net" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>ѡ��</th>
t  <th width=60%>����</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <tr><td><img src=pabb.gif>IP ��ַ</td>
c a i <td><input type=text name=ip value="%d.%d.%d.%d" size=18 maxlength=18></td></tr>
t <tr><td><IMG SRC=pabb.gif>��������</td>
c a m <td><input type=text name=msk value="%d.%d.%d.%d" size=18 maxlength=18></td></tr>
t <tr><td><IMG SRC=pabb.gif>Ĭ������</td>
c a g <td><input type=text name=gw value="%d.%d.%d.%d" size=18 maxlength=18></td></tr>
t <tr><td><IMG SRC=pabb.gif>��ѡDNS������</td>
c a p <td><input type=text name=pdns value="%d.%d.%d.%d" size=18 maxlength=18></td></tr>
t <tr><td><IMG SRC=pabb.gif>����DNS������</td>
c a s <td><input type=text name=sdns value="%d.%d.%d.%d" size=18 maxlength=18></td></tr>
t </font></table>
# Here begin button definitions
t <p align=center>
t <input type=button name=set value="ȷ��" onclick="changeConfirm(this.form)">
t <input type=reset value="ȡ��">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.

