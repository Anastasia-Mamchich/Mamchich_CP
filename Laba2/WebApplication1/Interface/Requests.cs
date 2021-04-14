using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using WebApplication1.Models;

namespace WebApplication1.Interface
{
    public class Requests : IRequests
    {
        public List<Ser> SelectFilmDuration(List<Ser> film)
        {
            var selected = film.Where(film => film.Video.Any(i => i.Duration < 40))
                .ToList();
            return selected;
        }

        public List<string> SelectNumberSersWithНighRating(List<Ser> film)
        {
            List<string> result = new List<string>();
            var selected = film.Where(film => film.Rating > 6).ToList();
            foreach (var i in selected)
            {
                var id = i.Id;
                var name = i.Name;
                var numb_ser = i.Video.Select(x => x.Series).Max();
                result.Add($"Номер сериала: {id} - Название сериала: {name} - Количество сезонов: {numb_ser}");
            }
            return result;
        }

        public List<string> SelectFresh(List<Ser> film)
        {
            List<string> result = new List<string>();
            var selected = film.Where(film => film.Fresh() == true).ToList();
            foreach (var i in selected)
            {
                var id = i.Id;
                var name = i.Name;
                string lang = "";
                foreach (var j in i.Video)
                {
                    var russ = j.Languages.Where(x => x.Name == "ru");
                    if (russ != null)
                    {
                      lang += $" {j.Name} --";
                    }
                }
                result.Add($"Номер сериала: {id} - Название сериала: {name} - Названия серий, где есть русская озвучка: --{lang}");
            }
            return result;
        }
        
        public List<string> SelectHowManyVideo(List<Ser> film)
        {
            List<string> result = new List<string>();
            foreach (var i in film)
            {
                var numb_ser = i.Video.Select(x => x.Series).Max();
                var name = i.Name;
                for (int j = 1; j <= numb_ser; j++)
                {
                    var video = i.Video.Where(x => x.Series == j);
                    var numb_video = video.Count();
                    var time = video.Sum(x => x.Duration)/60;
                    result.Add($"Название сериала: {name} - Сезон: {j}- Количество серий: {numb_video} - Время просмотра сезона: {string.Format("{0:N2}",time)}");
                }
            }
            return result;
        }

        public List<string> SelectHowManyTime(List<Ser> film)
        {
            List<string> result = new List<string>();
            foreach (var i in film)
            {
                var name = i.Name;
                var time = i.Video.Sum(x => x.Duration) / 60;
                result.Add($"Название сериала: {name} - Время просмотра сериала: {string.Format("{0:N2}", time)}");
            }
            return result;
        }

        public List<string> SelectShort(List<Ser> film)
        {
            List<string> result = new List<string>();
            foreach (var i in film)
            {
                var video = i.Video.Where(x => x.Duration <= 40).Select(z => z.Name);
                if (video.Count() != 0)
                {
                    result.Add($"Название сериала: {i.Name} - Названия серий: {String.Join(",", video)}");
                }
            }
            return result;
        }
    }
}
