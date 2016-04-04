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
    public class RootUnitConfigHandler : IConfigurationSectionHandler
    {
        public object Create(object parent, object configContext, XmlNode section)
        {
            XmlSerializer xs = new XmlSerializer(typeof(RootUnit));
            using (var xr = new XmlNodeReader(section))
            {
                var res = (RootUnit)xs.Deserialize(xr);
                return res;
            }

        }
    }
}
