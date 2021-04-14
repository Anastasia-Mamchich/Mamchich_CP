using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc.ModelBinding;
using System.ComponentModel.DataAnnotations.Schema;

namespace WebApplication1.Models
{
    public class Language
    {
        public long Id { get; set; }
        public string Name { get; set; }
        public bool Subtitles { get; set; }
        public long VideoId { get; set; }
    }

    public class Video
    {
        public long Id { get; set; }

        public float Duration { get; set; }

        public List<Language> Languages { get; set; } = new List<Language>();


        public string Name { get; set; }
        public int Series { get; set; } = 1;

        public long SerId { get; set; }

        public bool Short()
        {
            if (Duration <= 40)
            {
                return true;
            }

            else return false;
        }
    }
}
