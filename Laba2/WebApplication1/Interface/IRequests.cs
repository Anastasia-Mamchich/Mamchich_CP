using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using WebApplication1.Models;

namespace WebApplication1.Interface
{
    public interface IRequests
    {
        List<Ser> SelectFilmDuration(List<Ser> film);
        List<string> SelectNumberSersWithНighRating(List<Ser> film);
        List<string> SelectFresh(List<Ser> film);
        List<string> SelectHowManyVideo(List<Ser> film);
        List<string> SelectHowManyTime(List<Ser> film);

        List<string> SelectShort(List<Ser> film);

    }
}
