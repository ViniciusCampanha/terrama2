define([
  "TerraMA2WebApp/data-series/controllers",
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/data-series/schedule",
  "TerraMA2WebApp/data-series/components/index",
  "TerraMA2WebApp/data-series/directives"
], function(controllersApp, servicesApp, scheduleApp, componentsApp, directives) {
  var moduleName = "terrama2.dataseries";

  angular.module(moduleName, [controllersApp, servicesApp, scheduleApp, componentsApp, directives]);

  return moduleName;
});
