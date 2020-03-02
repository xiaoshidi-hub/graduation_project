t <html>
t <head>
t <meta http-equiv="Content-Type" content="text/html; charset=GBK" />
t <title>文件上传</title>
t </head>
i pg_header.inc
t <h2 align=center><br>WEB服务器文件上传</h2>
t <p><font size="3"><br>用户可以从电脑中选择要上传到WEB服务器的文件，注意，
t 当前仅做了短文件名支持，上传后将其存储到了SD卡的根目录下。</font></p>
t <form action=upload.cgi method=POST enctype="multipart/form-data" name=fileupload>
t <p><font face="verdana"><br><br><br>请选择要上传到WEB服务器的文件<br><br></font>
t  <input name="File" type="file" size="50" maxlength="1000">
t </p>
t <p align="center">
t  <input type=button value="上传文件" OnClick="this.form.submit()">
t </p>
i pg_footer.inc
. End of script must be closed with period.
