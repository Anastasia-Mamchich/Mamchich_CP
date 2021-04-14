using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using Seminar2.Models;

namespace Seminar2.Controllers
{
    [Route("api/Masters")]
    [ApiController]
    public class MastersController : ControllerBase
    {
        private readonly DBContext _context;

        public MastersController(DBContext context)
        {
            _context = context;
        }

        // GET: api/Masters
         [HttpGet]
    public async Task<ActionResult<IEnumerable<MasterDTO>>> GetMasters()
    {
        return await _context.Masters
            .Select(x => MasterToDTO(x))
            .ToListAsync();
    }

    [HttpGet("{id}")]
    public async Task<ActionResult<MasterDTO>> GetMaster(long id)
    {
        var master = await _context.Masters.FindAsync(id);

        if (master == null)
        {
            return NotFound();
        }

        return MasterToDTO(master);
    }

    [HttpPut("{id}")]
    public async Task<IActionResult> UpdateMaster(long id, MasterDTO masterDTO)
    {
        if (id != masterDTO.Id)
        {
            return BadRequest();
        }

        var master = await _context.Masters.FindAsync(id);
        if (master == null)
        {
            return NotFound();
        }

        master.Name = masterDTO.Name;
        master.Specialization = masterDTO.Specialization;

        try
        {
            await _context.SaveChangesAsync();
        }
        catch (DbUpdateConcurrencyException) when (!MasterExists(id))
        {
            return NotFound();
        }

        return NoContent();
    }

    [HttpPost]
    public async Task<ActionResult<MasterDTO>> CreateMaster(MasterDTO masterDTO)
    {
        var master = new Master
        {
            Specialization = masterDTO.Specialization,
            Name = masterDTO.Name
        };

        _context.Masters.Add(master);
        await _context.SaveChangesAsync();

        return CreatedAtAction(
            nameof(GetMaster),
            new { id = master.Id },
            MasterToDTO(master));
    }

    [HttpDelete("{id}")]
    public async Task<IActionResult> DeleteMaster(long id)
    {
        var master = await _context.Masters.FindAsync(id);

        if (master == null)
        {
            return NotFound();
        }

        _context.Masters.Remove(master);
        await _context.SaveChangesAsync();

        return NoContent();
    }

    private bool MasterExists(long id) =>
         _context.Masters.Any(e => e.Id == id);

    private static MasterDTO MasterToDTO(Master master) =>
        new MasterDTO
        {
            Id = master.Id,
            Name = master.Name,
            Specialization = master.Specialization
        };       
}
    
}
