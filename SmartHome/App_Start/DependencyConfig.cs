﻿using Autofac;
using Autofac.Integration.Mvc;
using Autofac.Integration.WebApi;
using SmartHome.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Web.Http;
using System.Web.Mvc;
using System.Web.UI;

namespace SmartHome
{
    public static class DependencyConfig
    {
        public static IContainer RegisterDependencyResolvers()
        {
            ContainerBuilder builder = new ContainerBuilder();

            RegisterDependencyMappingDefaults(builder);

            RegisterDependencyMappingOverrides(builder);

            IContainer container = builder.Build();
            
            // Set Up MVC Dependency Resolver
            DependencyResolver.SetResolver(new AutofacDependencyResolver(container));
            
            // Set Up WebAPI Resolver
            GlobalConfiguration.Configuration.DependencyResolver = new AutofacWebApiDependencyResolver(container);

            return container;
        }

        private static void RegisterDependencyMappingDefaults(ContainerBuilder builder)
        {
            /*
            Assembly coreAssembly = Assembly.GetAssembly(typeof(IStateManager));
            Assembly webAssembly = Assembly.GetAssembly(typeof(MvcApplication));

            builder.RegisterAssemblyTypes(coreAssembly).AsImplementedInterfaces().InstancePerRequest();
            builder.RegisterAssemblyTypes(webAssembly).AsImplementedInterfaces().InstancePerRequest();

            builder.RegisterControllers(webAssembly);
            builder.RegisterModule(new AutofacWebTypesModule());
            */

            builder.RegisterModule<AutofacWebTypesModule>();

            builder.RegisterControllers(Assembly.GetExecutingAssembly());

        }

        private static void RegisterDependencyMappingOverrides(ContainerBuilder builder)
        {            
            //builder.RegisterType<Fazenda>().As<IRootObject>().SingleInstance();
        }
    }
}
