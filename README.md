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
