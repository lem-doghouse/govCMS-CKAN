INTRODUCTION
============

govCMS CKAN provides integration with CKAN (http://ckan.org/). CKAN is a
powerful data management system that makes data accessible by providing tools to
streamline publishing, sharing, finding and using data.

GovCmsCkanClient Class
----------------------
This client class handles fetching, caching and returning data from a CKAN endpoint.

The CKAN base URL and API key are configured via the admin UI. When an endpoint request
is made it will first check if there is a local cached version and if not expired,
will return that. If no cache exists or expired a new request to the endpoint is performed
and cached.

Basic Usage of the class:
```
// Get a new instance of the class.
$client = govcms_ckan_client();

// Make a request.
$resource = 'action/package_show';
$query_params = array('id' => '1234');
$client->get($resource, $query_params);
```

This will make a request to the api which will look similar to this:
http://my.ckan.enpoint.baseurl/api/3/action/package_show?id=1234

Examples of resources:
action/package_show - metadata for a package
action/dataset - tabular data for a package

Submodules
----------

govCMS CKAN Display
- Handles turning the tabular data into charts.
- Provides ability to download charts as svg/png.

govCMS CKAN Display Examples
- Provides an examples page for viewing different chat types.