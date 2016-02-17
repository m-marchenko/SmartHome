using System;
using System.Collections.Generic;
using System.Linq;
using System.Web.Http;

namespace SmartHome
{
    public static class WebApiConfig
    {
        public static void Register(HttpConfiguration config)
        {
            // Web API configuration and services

            // Web API routes
            config.MapHttpAttributeRoutes();

            config.Routes.MapHttpRoute(
                name: "DefaultApi",
                routeTemplate: "api/{controller}/{id}",
                defaults: new { id = RouteParameter.Optional }
            );

            config.Routes.MapHttpRoute(
                name: "CommandApi",
                routeTemplate: "api/{controller}/{command}/{target}"
                //defaults: new { id = RouteParameter.Optional }
            );

            config.Routes.MapHttpRoute(
                    name: "CommandS",
                    routeTemplate: "api/{controller}/{sensorId}"
                    //defaults: new { id = RouteParameter.Optional }
                    );

        }
    }
}
