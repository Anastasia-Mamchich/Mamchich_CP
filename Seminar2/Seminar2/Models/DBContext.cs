using Microsoft.EntityFrameworkCore;

namespace Seminar2.Models
{
    public class DBContext : DbContext
    {
        public DBContext(DbContextOptions<DBContext> options)
            : base(options)
        {
        }

        public DbSet<Master> Masters { get; set; }
    }
}