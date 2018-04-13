using SmartHome.Models;
using SmartHome.Models.DataContracts;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Serialization;

namespace SmartHome.App_Start
{
    public class UserConfiguration
    {
        [XmlArrayItem(ElementName = "User")]
        public List<SmartHomeUser> Users { get; set; }

        [XmlArrayItem(ElementName = "RootUnit")]
        public List<RootUnit> Profiles { get; set; }
    }

    public class UserConfigHandler : IConfigurationSectionHandler
    {
        public object Create(object parent, object configContext, XmlNode section)
        {
            XmlSerializer xs = new XmlSerializer(typeof(UserConfiguration));
            using (var xr = new XmlNodeReader(section))
            {
                var res = (UserConfiguration)xs.Deserialize(xr);
                return res;
            }

        }
    }
    
}
