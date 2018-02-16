# bellcom_borgerdk

Installation and configuration
1. Activate module bellcom_borgerdk 
2. Go to Configuration->Borgerdk settings (admin/config/bellcom_borgerdk/settings) and select municipality for fetch content
3. Go to you installation route directory and run:
```
drush composer-json-rebuild
drush composer-manager install
```
4. Run drush scr scripts/bellcom_borgerdk.cron.php for import all articlies
