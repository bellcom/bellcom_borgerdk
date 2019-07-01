# Bellcom Borger.dk module installation and configuration

1. Activate module bellcom_borgerdk, via admin interface or via drush:
```
drush en bellcom_borgerdk
```
2. Go to Configuration->Borgerdk settings `admin/config/bellcom_borgerdk/settings` and select municipality for fetch content.
3. Go to you installation home directory and run:
```
drush composer-json-rebuild
drush composer-manager install
```

## To fast import all articles
1. In your terminal cd to the path where module is located.
2. Run the following command
```
drush scr scripts/bellcom_borgerdk.cron.php
```
3. Check that all the content is imported by going to `admin/content/borgerdk-articles`, and checking that no articles has asterix __(\*)__ next to it's name.

## To first cleanup microarticle and selfservices
Functionality for cleanup old microarticle and selfservices revisions was added into new version of bellcom_borgerdk moduel
If database tables borgerdk_microarticle_revision and borger_dk_selfservice_revision have a huge amount of records, recomended  to do cleanup for these tables directly in database at first
SQL queries for DB cleanup
```
delete from borgerdk_microarticle_revision where vid not in (select vid from borgerdk_microarticle) LIMIT 1000;
delete from borgerdk_selfservice_revision where vid not in (select vid from borgerdk_selfservice) LIMIT 1000;
```
