using System.Collections.Generic;
using Microsoft.AspNetCore.Mvc;
using Prometheus;

namespace HappinessRadar.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class HappinessController : ControllerBase
    {
        private static readonly Counter Happiness = Metrics.CreateCounter("happiness", "happiness", "location", "color");

        [HttpGet]
        public ActionResult<IEnumerable<string>> Get([FromQuery(Name = "location")] string location, [FromQuery(Name = "color")] string color)
        {
            Happiness.WithLabels(location, color).Inc();

            return Ok(new { location, color });
        }
    }
}
