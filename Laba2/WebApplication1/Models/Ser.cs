using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace WebApplication1.Models
{
    public class Ser
    {
        public long Id { get; set; }

        public string Genre { get; set; }

        public int Age_limit { get; set; }
        public string Name { get; set; }

        private float rating;
        public int Year { get; set; }
        public string Country { get; set; }
        
        public float Rating
        {
            set
            {
                if ((value <= 10) && (value >= 0))
                {
                    rating = value;
                }
                else rating = 0;
            }
            get { return rating; }
        }

        public bool Fresh()
        {
            if ((System.DateTime.Now.Year - Year) <= 1)
            {
                return true;
            }

            else return false;
        }

        public List<Video> Video { get; set; } = new List<Video>();    
    }
}
