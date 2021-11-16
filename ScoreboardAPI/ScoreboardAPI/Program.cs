using Microsoft.EntityFrameworkCore;

var builder = WebApplication.CreateBuilder(args);
builder.Services.AddDbContext<ScoreDb>(opt => opt.UseInMemoryDatabase("ScoreDB"));
builder.Services.AddDatabaseDeveloperPageExceptionFilter();
var app = builder.Build();

app.MapGet("/", () => "Hello World!");

app.MapGet("/scores", async (ScoreDb db) => await db.ScoreItems.ToListAsync());

app.MapPost("/scores", async (ScoreItem score, ScoreDb db) =>
{
    db.ScoreItems.Add(score);
    await db.SaveChangesAsync();

    return Results.Created($"/scores/{score.Id}", score);
});

app.Run();

class ScoreItem
{
    public int Id { get; set; }
    public string Name { get; set; }
    public int Score { get; set; }
}

class ScoreDb : DbContext
{
    public ScoreDb(DbContextOptions<ScoreDb> options) : base(options)
    {

    }

    public DbSet<ScoreItem> ScoreItems => Set<ScoreItem>();
}