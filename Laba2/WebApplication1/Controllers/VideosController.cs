using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using WebApplication1.Models;
using System.Text.Json;
using System.Text.Json.Serialization;
using Newtonsoft.Json;
using Microsoft.AspNetCore.Authorization;

namespace WebApplication1.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class VideosController : ControllerBase
    {
        private readonly Context _context;

        public VideosController(Context context)
        {
            _context = context;
        }

        // GET: api/Videos
        [Authorize]
        [HttpGet]
        public async Task<ActionResult<IEnumerable<Video>>> GetVideos()
        {
            var Films = _context.Videos.Include(x => x.Languages).
                ToListAsync();
            return await Films;
        }

        // GET: api/Videos/5
        [Authorize]
        [HttpGet("{id}")]
        public async Task<ActionResult<Video>> GetVideo(long id)
        {
            var video = _context.Videos.Include(i => i.Languages).FirstOrDefault(i => i.Id == id);

            if (video == null)
            {
                return NotFound();
            }

            return video;
        }

        // PUT: api/Videos/Name/5
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [Authorize(Roles = "admin")]
        [HttpPut("name/{id}")]
        public async Task<IActionResult> PutNameVideo(int id, Video value)
        {
            /*if (id != video.Id)
            {
                return BadRequest();
            }*/

            var video = _context.Videos.FirstOrDefault(i => i.Id == id);

            video.Name = value.Name;

            _context.Entry(video).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!VideoExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return Ok(video.Name);
        }

        [Authorize(Roles = "admin")]
        [HttpPut("series/{id}")]
        public async Task<IActionResult> PutSeriesVideo(int id, Video value)
        {
            /*if (id != video.Id)
            {
                return BadRequest();
            }*/

            var video = _context.Videos.FirstOrDefault(i => i.Id == id);

            video.Series = value.Series;

            _context.Entry(video).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!VideoExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return Ok(video.Series);
        }

        [Authorize(Roles = "admin")]
        [HttpPut("duration/{id}")]
        public async Task<IActionResult> PutDurationVideo(int id, Video value)
        {
            /*if (id != video.Id)
            {
                return BadRequest();
            }*/

            var video = _context.Videos.FirstOrDefault(i => i.Id == id);

            video.Duration = value.Duration;

            _context.Entry(video).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!VideoExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return Ok(video.Duration);
        }

        [Authorize(Roles = "admin")]
        [HttpPut("{id}/languages/edit/{id_lang}")]
        public async Task<IActionResult> EditLanguagesVideo(int id, int id_lang, [Bind("Name", "Subtitles")] Language value)
        {
            /*if (id != video.Id)
            {
                return BadRequest();
            }*/

            var video = _context.Videos.FirstOrDefault(i => i.Id == id);

            if (video != null)
            {
                var lang = _context.Languages.FirstOrDefault(i => ((i.Id == id_lang) && (i.VideoId == id)));
                var name_exists = _context.Languages.Any(i => ((i.VideoId == id) && (i.Name == value.Name)));

                if ((lang != null) && (name_exists == false))
                {
                    lang.Name = value.Name;
                    lang.Subtitles = value.Subtitles;
                }
                else return CreatedAtAction("AddLanguagesVideo", new { result = "Такая озвучка уже добавлена" });
            }

            _context.Entry(video).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!VideoExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return Ok(video.Languages);
        }

        [Authorize(Roles = "admin")]
        [HttpPost("{id}/languages/add")]
        public async Task<IActionResult> AddLanguagesVideo(int id,[Bind("Name", "Subtitles")] Language value)
        {
            /*if (id != video.Id)
            {
                return BadRequest();
            }*/

            var video = _context.Videos.FirstOrDefault(i => i.Id == id);

            if (video != null)
            {
                var lang = _context.Languages.Any(i => ((i.Name == value.Name) && (i.VideoId == id)));

                if (lang == false)
                {
                    value.VideoId = id;
                    _context.Languages.Add(value);
                }
                else return CreatedAtAction("AddLanguagesVideo", new { result = "Такая озвучка уже добавлена" });
            }

            _context.Entry(video).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!VideoExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return Ok(video.Languages);
        }

        // POST: api/Videos
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [HttpPost]

        [Authorize(Roles = "admin")]
        public async Task<ActionResult<Video>> PostVideo([FromBody] Video video)
        {
            if ((_context.Sers.Count()!=0) && (ModelState.IsValid))
            {
                _context.Videos.Add(video);
               await _context.SaveChangesAsync();

                return CreatedAtAction(nameof(GetVideo), new { id = video.Id }, video);
            }
            else return BadRequest();
        }

        // DELETE: api/Videos/5
        [Authorize(Roles = "admin")]
        [HttpDelete("{id}")]
        public async Task<ActionResult<Video>> DeleteVideo(long id)
        {
            var video = await _context.Videos.FindAsync(id);
            if (video == null)
            {
                return NotFound();
            }

            _context.Videos.Remove(video);
            await _context.SaveChangesAsync();

            return video;
        }

        [Authorize(Roles = "admin")]
        [HttpDelete("languages/del/{id_lang}")]
        public async Task<ActionResult<Language>> DeleteLanguagesVideo(long id_lang)
        {
            /*if (id != video.Id)
            {
                return BadRequest();
            }*/

            var lang = await _context.Languages.FindAsync(id_lang);
            if (lang == null)
            {
                return NotFound();
            }

            _context.Languages.Remove(lang);
            await _context.SaveChangesAsync();

            return lang;
        }

        private bool VideoExists(long id)
        {
            return _context.Videos.Any(e => e.Id == id);
        }
    }
}
