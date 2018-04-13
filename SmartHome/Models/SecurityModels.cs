using Microsoft.AspNet.Identity;
using SmartHome.App_Start;
using SmartHome.Models.DataContracts;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace SmartHome.Models
{

    [Serializable]
    public class SmartHomeUser : IUser
    {
        public SmartHomeUser()
        {
            Profiles = new List<RootUnit>();
        }
        public SmartHomeUser(string id, string name) : this()
        {
            Id = id;
            UserName = name;
        }

        //[XmlArrayItem(ElementName = "RootUnit")]
        [XmlIgnore]
        public List<RootUnit> Profiles { get; private set; }

        [XmlAttribute(AttributeName = "default")]
        public string DefaultProfileName { get; set; }

        [XmlAttribute(AttributeName = "id")]
        public string Id
        {
            get; set;

        }

        [XmlAttribute(AttributeName = "name")]
        public string UserName
        {
            get; set;
            
        }

        [XmlAttribute(AttributeName = "password")]
        public string Password { get; set; }
    }

    public class SmartHomeUserManager : UserManager<SmartHomeUser>
    {
         public SmartHomeUserManager() : base(new SmartHomeUserStore<SmartHomeUser>())
        {
            PasswordHasher = new LocalPasswordHasher();
        }


        public  override Task<SmartHomeUser> FindAsync(string userName, string password)
        {
            var task =  Task<SmartHomeUser>.Run( async () =>
          {

              //var user = this.Find(userName, password);

              var user = await this.Store.FindByNameAsync(userName);

              if (user != null)
              {
                  //First Verify Password...
                  PasswordVerificationResult result = this.PasswordHasher.VerifyHashedPassword(user.Password, password);

                  if (result == PasswordVerificationResult.SuccessRehashNeeded)
                  {
                      //Return User Profile Object...
                      //So this data object will come from Database we can write custom ADO.net to retrieve details/
                      //var user = await this.Store.FindByNameAsync(userName);

                      //SmartHomeUser user = new SmartHomeUser("1", userName);                    

                      return user;
                  }
              }
              return null;
          });
            return task;
        }

        private class LocalPasswordHasher : PasswordHasher
        {
            public override string HashPassword(string password)
            {
                return base.HashPassword(password);
            }

            public override PasswordVerificationResult VerifyHashedPassword(string hashedPassword, string providedPassword)
            {
              
                if (hashedPassword == providedPassword)
                    return PasswordVerificationResult.SuccessRehashNeeded;
                else    
                    return PasswordVerificationResult.Failed;

            }
        }
    }

    public class SmartHomeUserStore<T> : IUserStore<T> where T : SmartHomeUser, new()
    {
        private static UserConfiguration _config = (UserConfiguration)ConfigurationManager.GetSection("UserConfiguration");

        public SmartHomeUserStore()
        {

        }

        public Task CreateAsync(T user)
        {
            throw new NotImplementedException();
        }

        public Task DeleteAsync(T user)
        {
            throw new NotImplementedException();
        }

        public void Dispose()
        {
            //throw new NotImplementedException();
        }

        public Task<T> FindByIdAsync(string userId)
        {
            var user = _config.Users.OfType<T>().Where(u => u.Id == userId).FirstOrDefault();

            if (user != null)
            {
                user.Profiles.AddRange(_config.Profiles.Where(p => p.Name == user.DefaultProfileName));
            }

            return Task.FromResult<T>(user);
        }

        public Task<T> FindByNameAsync(string userName)
        {
            var user = _config.Users.OfType<T>().Where(u => u.UserName == userName).FirstOrDefault();

            if (user != null)
            {
                user.Profiles.AddRange(_config.Profiles.Where(p => p.Name == user.DefaultProfileName));
            }

            return Task.FromResult<T>(user);
        }

        public Task UpdateAsync(T user)
        {
            throw new NotImplementedException();
        }
    }
}
