Welcome to BoviBook

This is a dairy herd management web app. 

See: https://github.com/wandc/bovibook/wiki

Install on Amazon EC2 instance:

Choose Linux Ubuntu for OS.

#sudo apt-get install php7.0 apache2
#sudo apt-get install composer
#sudo apt-get install curl
#sudo apt-get install php libapache2-mod-php
#sudo apt-get install php7.2-gd
#sudo apt-get install php7.0-mbstring
#sudo apt-get install php7.0-zip


#cd /var/www/html

#sudo composer create-project wandc/bovibook:dev-master
Do you want to remove the existing VCS (.git, .svn..) history? [Y,n]?
YES

CHANGE PERMISSION on bovibook dir 744? check

go edit apache2 config

 sudo nano /etc/apache2/sites-enabled/000-default.conf

change line:
 DocumentRoot /var/www/html/
toL:
 DocumentRoot /var/www/html/bovibook

restart apache

sudo systemctl restart apache2

If everything went well you can go to your URL and should see bovibook login.

If you do not, go check less /var/log/apache2/error.log for errors

Currently we bovibook uses HTML_QuickForm. This is deprecated and does not work directly with php7. It can be made to work, by editing the files. It needs to be removed.
#sudo pear install HTML_QuickForm
