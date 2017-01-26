using Microsoft.AspNet.Identity;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmartHome.Models
{
    public class SmartHomeUser : IUser
    {
        public SmartHomeUser()
        {
            Id = "1";
        }
        public SmartHomeUser(string id, string name)
        {
            Id = id;
            UserName = name;
        }

        public string Id
        {
            get; private set;

        }

        public string UserName
        {
            get; set;
            
        }
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
                  PasswordVerificationResult result = this.PasswordHasher.VerifyHashedPassword(userName, password);

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
              

                    return PasswordVerificationResult.SuccessRehashNeeded;
                    
                //return PasswordVerificationResult.Failed;

            }
        }
    }

    public class SmartHomeUserStore<T> : IUserStore<T> where T : SmartHomeUser, new()
    {
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
            throw new NotImplementedException();
        }

        public Task<T> FindByNameAsync(string userName)
        {
            return Task.FromResult<T>(new T() { UserName = userName });


        }

        public Task UpdateAsync(T user)
        {
            throw new NotImplementedException();
        }
    }
}
