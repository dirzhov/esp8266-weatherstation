@echo off
SET main_min_js=main.min.js
SET main_min_css=main.min.css
java -jar htmlcompressor-1.5.3.jar -o ./data ./web/index.html
java -jar yuicompressor-2.4.8.jar web/main.js -o "web\%main_min_js%"
java -jar yuicompressor-2.4.8.jar web/main.css -o "web\%main_min_css%"
del ".\data\%main_min_js%"
del ".\data\%main_min_css%"
type ".\web\moment.min.js" >> data/%main_min_js%
type ".\web\Chart.min.js" >> data/%main_min_js%
type ".\web\zepto.min.js" >> data/%main_min_js%
type ".\web\tabbedcontent.min.js" >> data/%main_min_js%
type ".\web\main.min.js" >> data/%main_min_js%
type ".\web\%main_min_css%" >> data/%main_min_css%
rem for /r "web" %%F in (*.min.js) do type "%%F" >> data/main.min.js
rem for /r "web" %%F in (*.min.css) do type "%%F" >> data/main.min.css
gzip -r -9 -f data/*
