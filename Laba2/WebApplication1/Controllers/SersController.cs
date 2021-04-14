using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using WebApplication1.Models;
using WebApplication1.Interface;
using Microsoft.AspNetCore.Authorization;

namespace WebApplication1.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class SersController : ControllerBase
    {
        private readonly Context _context;
        private readonly IRequests _request;

        public SersController(Context context, IRequests request)
        {
            _context = context;
            _request = request;
        }

        // GET: api/Films
        [Authorize]
        [HttpGet]
        public async Task<ActionResult<IEnumerable<Ser>>> GetSers()
        {
            var Films = _context.Sers.Include(x => x.Video).ThenInclude(z => z.Languages);
            return await Films.ToListAsync();
        }


        // GET: api/Films/5
        [Authorize]
        [HttpGet("{id}")]
        public async Task<ActionResult<Ser>> GetSer(long id)
        {
            var film = _context.Sers.Include(i => i.Video).ThenInclude(z => z.Languages).FirstOrDefault(i => i.Id == id);

            if (film == null)
            {
                return NotFound();
            }

            return film;
        }

        // GET: api/Films/rating
        [Authorize]
        [HttpGet("rating")]
        public CreatedAtActionResult Rating()
        {
            var film = _request.SelectNumberSersWithНighRating(_context.Sers.Include(i => i.Video).ToList());
            if (film.Count == 0)
                return CreatedAtAction("rating", new
                { result = "Не найдено сериалов с высоким рейтингом" });
            else
            {
                return CreatedAtAction("rating", new
                { film });
            }
        }

        // GET: api/Films/fresh
        [Authorize]
        [HttpGet("fresh")]
        public CreatedAtActionResult FreshSers()
        {
            var film = _request.SelectFresh(_context.Sers.Include(i => i.Video).ThenInclude(x => x.Languages).ToList());
            if (film.Count == 0)
                return CreatedAtAction("fresh", new
                { result = "Не найдено новинок" });
            else
            {
                return CreatedAtAction("fresh", new
                { film });
            }
        }

        // GET: api/Films/timeseries
        [Authorize]
        [HttpGet("timeseries")]
        public CreatedAtActionResult HowManyVideo()
        {
            var film = _request.SelectHowManyVideo(_context.Sers.Include(i => i.Video).ThenInclude(x => x.Languages).ToList());
            if (film.Count == 0)
                return CreatedAtAction("timeseries", new
                { result = "Не найдено данных" });
            else
            {
                return CreatedAtAction("timeseries", new
                { film });
            }
        }

        // GET: api/Films/time
        [Authorize]
        [HttpGet("time")]
        public CreatedAtActionResult SelectTime()
        {
            var film = _request.SelectHowManyTime(_context.Sers.Include(i => i.Video).ThenInclude(x => x.Languages).ToList());
            if (film.Count == 0)
                return CreatedAtAction("time", new
                { result = "Не найдено новинок" });
            else
            {
                return CreatedAtAction("time", new
                { film });
            }
        }

        // GET: api/Films/short
        [Authorize]
        [HttpGet("short")]
        public CreatedAtActionResult SelectShortSers()
        {
            var film = _request.SelectShort(_context.Sers.Include(i => i.Video).ThenInclude(x => x.Languages).ToList());
            if (film.Count == 0)
                return CreatedAtAction("short", new
                { result = "Не найдено короткометражек" });
            else
            {
                return CreatedAtAction("short", new
                { film });
            }
        }

        // PUT: api/Films/5
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [Authorize(Roles = "admin")]
        [HttpPut("{id}")]
        public async Task<IActionResult> PutSer(long id, Ser value)
        {
            /*if (id != film.Id)
            {
                return BadRequest();
            }*/


            var cont = _context.Sers.Include(x => x.Video).ThenInclude(z => z.Languages);
            var ser = cont.FirstOrDefault(i => i.Id == id);

            ser.Name = value.Name;
            ser.Rating = value.Rating;
            ser.Year = value.Year;
            ser.Country = value.Country;
            ser.Genre = value.Genre;
            ser.Age_limit = value.Age_limit;

            _context.Entry(ser).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!SerExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return Ok(ser);
        }


        // POST: api/Films
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        

        [Authorize(Roles = "admin")]
        [HttpPost]
        public async Task<ActionResult<Ser>> PostSer(Ser film)
        {
            _context.Sers.Add(film);
            await _context.SaveChangesAsync();

            return CreatedAtAction(nameof(GetSer), new { id = film.Id }, film);
        }

        // DELETE: api/Films/5
        [Authorize(Roles = "admin")]
        [HttpDelete("{id}")]
        public async Task<ActionResult<Ser>> DeleteSer(long id)
        {
            var film = await _context.Sers.FindAsync(id);
            if (film == null)
            {
                return NotFound();
            }

            _context.Sers.Remove(film);
            await _context.SaveChangesAsync();

            return film;
        }

      

        private bool SerExists(long id)
        {
            return _context.Sers.Any(e => e.Id == id);
        }
    }
}
